#include "plugin.hpp"
#include "../helpers/dsp_utils.hpp"   // <-- tus utilidades

using namespace rack;
using namespace DSPUtils;

// -----------------------------
// SVF simple (Zavalishin) LP
// -----------------------------
struct SVF {
	float ic1eq = 0.f, ic2eq = 0.f;
	float g = 0.f;
	float k = 0.6f;

	void setParams(float cutoff, float sr, float resonance = 0.6f) {
		cutoff = clamp(cutoff, 20.f, 12000.f);
		k = clamp(resonance, 0.05f, 1.2f);
		float x = (float)M_PI * cutoff / sr;
		g = std::tan(x);
	}

	void reset() { ic1eq = ic2eq = 0.f; }

	float processLP(float v0) {
		// TPT SVF
		float a1 = 1.f / (1.f + g*(g + k));
		float v1 = (v0 - ic2eq - k*ic1eq) * a1;
		float v2 = g*v1 + ic1eq;
		float v3 = g*v2 + ic2eq;
		ic1eq = 2.f*v2 - ic1eq;
		ic2eq = 2.f*v3 - ic2eq;
		return v3; // LP
	}
};

// -----------------------------
// Oscilador polyBLEP (saw/square)
// -----------------------------
struct PolyBLEPOsc {
	float phase = 0.f;   // [0,1)
	float freq = 100.f;  // Hz
	float sr = 44100.f;

	void setSampleRate(float s) { sr = s; }
	void setFreq(float f) { freq = clamp(f, 10.f, 12000.f); }
	void resetPhase() { phase = 0.f; }

	inline float polyblep(float t, float dt) {
		if (t < dt) {
			t /= dt;
			return t + t - t*t - 1.f;
		}
		else if (t > 1.f - dt) {
			t = (t - 1.f) / dt;
			return t*t + t + t + 1.f;
		}
		return 0.f;
	}

	float processSaw() {
		float dt = freq / sr;
		phase += dt;
		if (phase >= 1.f) phase -= 1.f;

		float out = 2.f * phase - 1.f;
		out -= polyblep(phase, dt);
		return out;
	}

	float processSquare(float pwm = 0.5f) {
		pwm = clamp(pwm, 0.05f, 0.95f);
		float dt = freq / sr;
		phase += dt;
		if (phase >= 1.f) phase -= 1.f;

		float out = (phase < pwm) ? 1.f : -1.f;
		out += polyblep(phase, dt);
		float t = std::fmod(phase - pwm + 1.f, 1.f);
		out -= polyblep(t, dt);
		return out;
	}
};

// -----------------------------
// Módulo
// -----------------------------
struct TL_Bass : Module {
	enum ParamId {
		BTN_TRIG_PARAM,        // botón
		FILTER_KNOB_PARAM,     // -10..+10 (octavas relativas)
		DECAY_KNOB_PARAM,      // -10..+10 (usa tu DecayEnvelope)
		TIMBRE_SELECTOR_PARAM, // 0=2, 1=1 (según el serigrafiado)
		PARAMS_LEN
	};
	enum InputId {
		TRIGGER_JACK_INPUT,
		VOCT_JACK_INPUT,
		FILTER_JACK_INPUT,   // 1V/oct para cutoff
		DECAY_JACK_INPUT,    // suma al knob (en “unidades de knob”)
		INPUTS_LEN
	};
	enum OutputId {
		OUT_MONO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BTN_TRIG_LIGHT,
		LIGHTS_LEN
	};

	// DSP state
	dsp::SchmittTrigger trigIn, trigBtn;
	PolyBLEPOsc osc;
	SVF svf;

	// Envolventes
	DecayEnvelope env; // tu envolvente principal (one-shot)
	float env2 = 0.f;  // transitorio corto para punch
	float env2Coeff = 0.999f;

	// DC block usando tu HighPassFilter
	HighPassFilter dcBlock;

	TL_Bass() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(BTN_TRIG_PARAM, 0.f, 1.f, 0.f, "Trigger");
		configParam(FILTER_KNOB_PARAM, -10.f, 10.f, 0.f, "Filter cutoff (octaves)");
		configParam(DECAY_KNOB_PARAM, -10.f, 10.f, 0.f, "Decay");
		configSwitch(TIMBRE_SELECTOR_PARAM, 0.f, 1.f, 1.f, "Timbre", {"2", "1"});

		configInput(TRIGGER_JACK_INPUT, "Trigger");
		configInput(VOCT_JACK_INPUT, "V/Oct");
		configInput(FILTER_JACK_INPUT, "Filter");
		configInput(DECAY_JACK_INPUT, "Decay");
		configOutput(OUT_MONO_OUTPUT, "Mono");
	}

	void onSampleRateChange() override {
		float sr = APP->engine->getSampleRate();
		osc.setSampleRate(sr);
		// DC-block ~20 Hz
		dcBlock.setCutoff(20.f, sr);
	}

	// Mapear FILTER a cutoff (octavas relativas, 1V/oct por jack)
	static inline float cutoffFromControls(float filterKnob, float filterCV, float keytrack) {
		// referencia 40 Hz
		float oct = (filterKnob * 0.25f) + filterCV + keytrack; // knob ±2.5 oct
		return 40.f * std::pow(2.f, oct);
	}

	void process(const ProcessArgs& args) override {
		float sr = args.sampleRate;

		// --- triggers (jack + botón)
		bool fired = false;
		if (inputs[TRIGGER_JACK_INPUT].isConnected())
			fired |= trigIn.process(inputs[TRIGGER_JACK_INPUT].getVoltage());
		fired |= trigBtn.process(params[BTN_TRIG_PARAM].getValue());

		lights[BTN_TRIG_LIGHT].setBrightnessSmooth(fired ? 1.f : 0.f, args.sampleTime);

		// --- Decay param total (knob + CV en “unidades de knob”)
		float decayKnob = params[DECAY_KNOB_PARAM].getValue();
		float decayCV   = inputs[DECAY_JACK_INPUT].isConnected() ? inputs[DECAY_JACK_INPUT].getVoltage() : 0.f;
		float decayParam = clamp(decayKnob + decayCV, -10.f, 10.f);

		// Envolvente secundaria (transitorio ~3 ms)
		env2Coeff = std::exp(-1.f / (0.003f * sr));

		if (fired) {
			env.trigger(decayParam, sr); // usa tu DecayEnvelope
			env2 = 1.f;
			osc.resetPhase(); // ataque consistente
		}

		// --- Pitch 1 V/oct (0 V = 440 Hz)
		float pitchV = inputs[VOCT_JACK_INPUT].getVoltage();
		float freq = 440.f * std::pow(2.f, pitchV);

		// Modo/timbre
		bool mode1 = params[TIMBRE_SELECTOR_PARAM].getValue() > 0.5f;

		// Ligerísimo “pitch bend” inicial en modo 2
		float pitchBend = mode1 ? 1.f : (1.f + 0.01f * env2);
		osc.setFreq(clamp(freq * pitchBend, 10.f, 12000.f));

		// --- Osc
		float oscSample = mode1 ? osc.processSaw() : osc.processSquare(0.48f);

		// --- Drive previo al filtro
		float drive = mode1 ? 1.2f : 1.6f;
		float pre = std::tanh(drive * oscSample);

		// --- Envolventes
		float e  = env.process();       // tu decay
		env2 *= env2Coeff;

		// --- Cutoff (knob + 1V/oct por jack + keytracking 50% + env to filter)
		float filterKnob = params[FILTER_KNOB_PARAM].getValue();
		float filterCVoct = inputs[FILTER_JACK_INPUT].isConnected() ? inputs[FILTER_JACK_INPUT].getVoltage() : 0.f;
		float keytrack = 0.5f * pitchV;
		float envToFilterOct = 1.5f * e;

		float cutoff = cutoffFromControls(filterKnob, filterCVoct + envToFilterOct, keytrack);
		svf.setParams(cutoff, sr, mode1 ? 0.6f : 0.8f);

		float lp = svf.processLP(pre);

		// --- VCA
		float out = lp * e;

		// --- DC-block con tu HighPassFilter y normalizar a ±5 V
		out = dcBlock.process(out);       // usa tu HPF (una sola entrada)
		out = clamp(out * 5.f, -5.f, 5.f);

		outputs[OUT_MONO_OUTPUT].setVoltage(out);
	}
};

// -----------------------------
// Widget (coincide con tu panel)
// -----------------------------
struct TL_BassWidget : ModuleWidget {
	TL_BassWidget(TL_Bass* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Bass.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Botón con luz en la parte superior (coords según tu SVG)
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(15.142, 24.404)), module, TL_Bass::BTN_TRIG_PARAM, TL_Bass::BTN_TRIG_LIGHT));

		// Knobs
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(15.056, 46.994)), module, TL_Bass::FILTER_KNOB_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(14.933, 68.928)), module, TL_Bass::DECAY_KNOB_PARAM));

		// Switch 1/2
		addParam(createParamCentered<CKSS>(mm2px(Vec(7.957, 109.690)), module, TL_Bass::TIMBRE_SELECTOR_PARAM));

		// Entradas
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.297, 28.152)), module, TL_Bass::TRIGGER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.206, 28.152)), module, TL_Bass::VOCT_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.891, 92.414)), module, TL_Bass::FILTER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.650, 92.422)), module, TL_Bass::DECAY_JACK_INPUT));

		// Salida
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(22.638, 108.322)), module, TL_Bass::OUT_MONO_OUTPUT));
	}
};

Model* modelTL_Bass = createModel<TL_Bass, TL_BassWidget>("TL_Bass");
