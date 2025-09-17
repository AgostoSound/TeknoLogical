#include "plugin.hpp"
#include "../helpers/dsp_utils.hpp"

using namespace rack;
using namespace DSPUtils;

// -----------------------------
// Oscilador polyBLEP (saw/square) + sub-sine
// -----------------------------
struct PolyBLEPOsc {
	float phase = 0.f;    // [0,1)
	float subPhase = 0.f; // sub a -1 oct
	float freq = 100.f;   // Hz
	float sr = 44100.f;

	void setSampleRate(float s) { sr = s; }
	void setFreq(float f) { freq = clamp(f, 10.f, 12000.f); }
	void resetPhase() { phase = 0.f; subPhase = 0.f; }

	inline float polyblep(float t, float dt) {
		if (t < dt) { t /= dt; return t + t - t*t - 1.f; }
		if (t > 1.f - dt) { t = (t - 1.f) / dt; return t*t + t + t + 1.f; }
		return 0.f;
	}

	inline void advance() {
		float dt = freq / sr;
		phase += dt; if (phase >= 1.f) phase -= 1.f;
		// sub una octava abajo
		float subDt = 0.5f * dt;
		subPhase += subDt; if (subPhase >= 1.f) subPhase -= 1.f;
	}

	float processSaw() {
		float dt = freq / sr;
		advance();
		float out = 2.f * phase - 1.f;
		out -= polyblep(phase, dt);
		return out;
	}

	float processSquare(float pwm = 0.5f) {
		pwm = clamp(pwm, 0.05f, 0.95f);
		float dt = freq / sr;
		advance();
		float out = (phase < pwm) ? 1.f : -1.f;
		out += polyblep(phase, dt);
		float t = std::fmod(phase - pwm + 1.f, 1.f);
		out -= polyblep(t, dt);
		return out;
	}

	inline float subSine() const {
		return std::sin(2.f * M_PI * subPhase);
	}
};

// -----------------------------
// Módulo
// -----------------------------
struct TL_Bass : Module {
	enum ParamId {
		BTN_TRIG_PARAM,
		FILTER_KNOB_PARAM,     // -10..+10  (0=bypass; <0 LP; >0 HP)
		DECAY_KNOB_PARAM,      // -10..+10  (usa tu DecayEnvelope)
		TIMBRE_SELECTOR_PARAM, // “1 / 2”
		PARAMS_LEN
	};
	enum InputId {
		TRIGGER_JACK_INPUT,
		VOCT_JACK_INPUT,       // limitado a ±2 oct
		FILTER_JACK_INPUT,     // suma al knob, mismo rango -10..+10
		DECAY_JACK_INPUT,
		INPUTS_LEN
	};
	enum OutputId { OUT_MONO_OUTPUT, OUTPUTS_LEN };
	enum LightId { BTN_TRIG_LIGHT, LIGHTS_LEN };

	// Estado DSP
	dsp::SchmittTrigger trigIn, trigBtn;
	PolyBLEPOsc osc;

	// Envolventes
	DecayEnvelope env;
	float env2 = 0.f;     // transitorio para “punch”
	float env2Coeff = 0.999f;

	// Filtros como en TL-Drum5
	CachedLowPass  lowFilter;
	CachedHighPass highFilter;

	// DC-block
	HighPassFilter dcBlock;

	// Gestión V/Oct
	bool voctWasConnected = false;

	// 0 V = 440 Hz; queremos C2 ≈ 65.406 Hz como “default”.
	// offset en voltios: log2(65.406/440) ≈ -2.75 V
	static constexpr float BASE_V_DEFAULT = -2.75f;

	TL_Bass() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BTN_TRIG_PARAM, 0.f, 1.f, 0.f, "Trigger");
		configParam(FILTER_KNOB_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(DECAY_KNOB_PARAM, -10.f, 10.f, 0.f, "Decay");
		configSwitch(TIMBRE_SELECTOR_PARAM, 0.f, 1.f, 1.f, "Timbre", {"2", "1"});

		configInput(TRIGGER_JACK_INPUT, "Trigger");
		configInput(VOCT_JACK_INPUT,    "V/Oct (±2 oct)");
		configInput(FILTER_JACK_INPUT,  "Filter CV");
		configInput(DECAY_JACK_INPUT,   "Decay CV");
		configOutput(OUT_MONO_OUTPUT,   "Mono");
	}

	void onSampleRateChange() override {
		float sr = APP->engine->getSampleRate();
		osc.setSampleRate(sr);
		dcBlock.setCutoff(20.f, sr);
	}

	void process(const ProcessArgs& args) override {
		float sr = args.sampleRate;

		// --- Trigger por jack y botón
		bool fired = false;
		if (inputs[TRIGGER_JACK_INPUT].isConnected())
			fired |= trigIn.process(inputs[TRIGGER_JACK_INPUT].getVoltage());
		fired |= trigBtn.process(params[BTN_TRIG_PARAM].getValue());

		// Detectar desconexión de V/Oct → disparo “default”
		bool voctNow = inputs[VOCT_JACK_INPUT].isConnected();
		if (voctWasConnected && !voctNow) {
			fired = true;                // re-dispara al desconectar
		}
		voctWasConnected = voctNow;

		lights[BTN_TRIG_LIGHT].setBrightnessSmooth(fired ? 1.f : 0.f, args.sampleTime);

		// --- Decay (knob + CV)
		float decayParam = clamp(params[DECAY_KNOB_PARAM].getValue()
			+ (inputs[DECAY_JACK_INPUT].isConnected() ? inputs[DECAY_JACK_INPUT].getVoltage() : 0.f),
			-10.f, 10.f);

		// Envolvente secundaria (≈3 ms)
		env2Coeff = std::exp(-1.f / (0.003f * sr));
		if (fired) {
			env.trigger(decayParam, sr);
			env2 = 1.f;
			osc.resetPhase();
		}

		// --- Pitch 1 V/oct limitado a ±2 oct
		float pitchIn = voctNow ? clamp(inputs[VOCT_JACK_INPUT].getVoltage(), -2.f, 2.f) : 0.f;
		float pitchV  = BASE_V_DEFAULT + pitchIn;  // base C2 + desviación limitada
		float freq    = 440.f * std::pow(2.f, pitchV);

		// Timbre
		bool mode1 = params[TIMBRE_SELECTOR_PARAM].getValue() > 0.5f; // “1” clásico / “2” agresivo

		// Ligerísimo pitch-bend inicial en modo 2
		float bend = mode1 ? 1.f : (1.f + 0.01f * env2);
		osc.setFreq(clamp(freq * bend, 10.f, 12000.f));

		// --- Osc + sub
		float oscSample = mode1 ? osc.processSaw() : osc.processSquare(0.48f);
		float sub = osc.subSine(); // -1 oct
		// Mezcla “techno”: modo1 con sub presente; modo2 casi sin sub
		float pre = (mode1 ? (0.75f * oscSample + 0.25f * sub)
		                   : (0.90f * oscSample + 0.10f * sub));

		// Drive antes del filtro
		pre = std::tanh((mode1 ? 1.35f : 1.7f) * pre);

		// --- Envolventes
		float e = env.process();
		env2 *= env2Coeff;

		// --- Filtro (contrato TL-Drum5): -10..0 LP | 0..+10 HP | 0=bypass
		float filterVal = params[FILTER_KNOB_PARAM].getValue()
			+ (inputs[FILTER_JACK_INPUT].isConnected() ? inputs[FILTER_JACK_INPUT].getVoltage() : 0.f);
		filterVal = clamp(filterVal, -10.f, 10.f);

		// Tus filtros cacheados: aplicar ambos como en TL-Drum5
		float x = pre;
		x = lowFilter.process(x,  filterVal, sr);
		x = highFilter.process(x, filterVal, sr);

		// VCA
		float out = x * e;

		// DC-block y normalización ±5 V
		out = dcBlock.process(out);
		out = clamp(out * 5.f, -5.f, 5.f);

		outputs[OUT_MONO_OUTPUT].setVoltage(out);
	}
};

// -----------------------------
// Widget (sin cambios)
// -----------------------------
struct TL_BassWidget : ModuleWidget {
	TL_BassWidget(TL_Bass* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Bass.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(15.142, 24.404)), module, TL_Bass::BTN_TRIG_PARAM, TL_Bass::BTN_TRIG_LIGHT));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(15.056, 46.994)), module, TL_Bass::FILTER_KNOB_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(14.933, 68.928)), module, TL_Bass::DECAY_KNOB_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(7.957, 109.690)), module, TL_Bass::TIMBRE_SELECTOR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.297, 28.152)), module, TL_Bass::TRIGGER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.206, 28.152)), module, TL_Bass::VOCT_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.891, 92.414)), module, TL_Bass::FILTER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.650, 92.422)), module, TL_Bass::DECAY_JACK_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(22.638, 108.322)), module, TL_Bass::OUT_MONO_OUTPUT));
	}
};

Model* modelTL_Bass = createModel<TL_Bass, TL_BassWidget>("TL_Bass");
