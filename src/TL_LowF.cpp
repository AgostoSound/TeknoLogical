#include "plugin.hpp"
#include "../helpers/widgets/sliders.hpp"

struct TL_LowF : Module {
	enum ParamId {
		// General controls
		AMP_KNOB_PARAM,
		FREQ_KNOB_PARAM,

		// Multipliers
		MULTIPLO1_PARAM,
		MULTIPLO2_PARAM,
		MULTIPLO3_PARAM,
		MULTIPLO4_PARAM,

		// Waves
		ONDA1_PARAM,
		ONDA2_PARAM,
		ONDA3_PARAM,
		ONDA4_PARAM,

		PARAMS_LEN
	};

	enum InputId {
		AMP_CV_INPUT,
		FREQ_CV_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LED1_LIGHT,
		LED2_LIGHT,
		LED3_LIGHT,
		LED4_LIGHT,
		LIGHTS_LEN
	};

	float phase = 0.f;

	TL_LowF() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// General controls
		configParam(FREQ_KNOB_PARAM, -8.f, 10.f, 1.f, "Frequency", " Hz", 2.f, 1.f);

		// AMP is peak amplitude in volts, 0..5V, default 2.5V (center/up position)
		configParam(AMP_KNOB_PARAM, 0.f, 5.f, 2.5f, "Amplitude", " V");

		// Multipliers
		configSwitch(MULTIPLO1_PARAM, 0.f, 2.f, 0.f, "Multiplier 1", {"x1", "x2", "x3"});
		configSwitch(MULTIPLO2_PARAM, 0.f, 2.f, 0.f, "Multiplier 2", {"x1", "x2", "x3"});
		configSwitch(MULTIPLO3_PARAM, 0.f, 2.f, 0.f, "Multiplier 3", {"x1", "x2", "x3"});
		configSwitch(MULTIPLO4_PARAM, 0.f, 2.f, 0.f, "Multiplier 4", {"x1", "x2", "x3"});

		// Waves
		configSwitch(ONDA1_PARAM, 0.f, 2.f, 0.f, "Wave 1", {"Sine", "Triangle", "Square"});
		configSwitch(ONDA2_PARAM, 0.f, 2.f, 0.f, "Wave 2", {"Sine", "Triangle", "Square"});
		configSwitch(ONDA3_PARAM, 0.f, 2.f, 0.f, "Wave 3", {"Sine", "Triangle", "Square"});
		configSwitch(ONDA4_PARAM, 0.f, 2.f, 0.f, "Wave 4", {"Sine", "Triangle", "Square"});

		// Inputs
		configInput(AMP_CV_INPUT, "Amplitude CV");
		configInput(FREQ_CV_INPUT, "Frequency CV");

		// Outputs
		configOutput(OUT1_OUTPUT, "Channel 1");
		configOutput(OUT2_OUTPUT, "Channel 2");
		configOutput(OUT3_OUTPUT, "Channel 3");
		configOutput(OUT4_OUTPUT, "Channel 4");
	}

	static float triangleWave(float p) {
		// p in [0, 1)
		// output in [-1, 1]
		return 1.f - 4.f * std::fabs(p - 0.5f);
	}

	static float squareWave(float p) {
		return (p < 0.5f) ? 1.f : -1.f;
	}

	static float sineWave(float p) {
		return std::sin(2.f * M_PI * p);
	}

	static float evalWave(int waveType, float p) {
		switch (waveType) {
			case 0:  return sineWave(p);
			case 1:  return triangleWave(p);
			case 2:  return squareWave(p);
			default: return sineWave(p);
		}
	}

	// Main cycle.
	void process(const ProcessArgs& args) override {
		// Frequency control
		// If CV is connected, ignore knob.
		float freqControl;
		if (inputs[FREQ_CV_INPUT].isConnected()) {
			// 0..10V mapped to -8..10, same full travel as knob
			float cv = clamp(inputs[FREQ_CV_INPUT].getVoltage(), 0.f, 10.f);
			freqControl = rescale(cv, 0.f, 10.f, -8.f, 10.f);
		}
		else {
			freqControl = params[FREQ_KNOB_PARAM].getValue();
		}

		float baseFreq = std::pow(2.f, freqControl);
		baseFreq = clamp(baseFreq, 0.f, 1024.f);

		// Amplitude control
		// If CV is connected, ignore knob.
		float amplitude;
		if (inputs[AMP_CV_INPUT].isConnected()) {
			// 0..10V mapped to 0..5V
			float cv = clamp(inputs[AMP_CV_INPUT].getVoltage(), 0.f, 10.f);
			amplitude = rescale(cv, 0.f, 10.f, 0.f, 5.f);
		}
		else {
			amplitude = params[AMP_KNOB_PARAM].getValue();
		}
		amplitude = clamp(amplitude, 0.f, 5.f);

		// Advance master phase with base frequency
		phase += baseFreq * args.sampleTime;
		phase -= std::floor(phase);

		// Read stepped params
		int mults[4] = {
			(int) std::round(params[MULTIPLO1_PARAM].getValue()) + 1,
			(int) std::round(params[MULTIPLO2_PARAM].getValue()) + 1,
			(int) std::round(params[MULTIPLO3_PARAM].getValue()) + 1,
			(int) std::round(params[MULTIPLO4_PARAM].getValue()) + 1
		};

		int waves[4] = {
			(int) std::round(params[ONDA1_PARAM].getValue()),
			(int) std::round(params[ONDA2_PARAM].getValue()),
			(int) std::round(params[ONDA3_PARAM].getValue()),
			(int) std::round(params[ONDA4_PARAM].getValue())
		};

		const float phaseOffsets[4] = {
			0.00f,  //   0°
			0.25f,  //  90°
			0.50f,  // 180°
			0.75f   // 270°
		};

		Output* outs[4] = {
			&outputs[OUT1_OUTPUT],
			&outputs[OUT2_OUTPUT],
			&outputs[OUT3_OUTPUT],
			&outputs[OUT4_OUTPUT]
		};

		Light* leds[4] = {
			&lights[LED1_LIGHT],
			&lights[LED2_LIGHT],
			&lights[LED3_LIGHT],
			&lights[LED4_LIGHT]
		};

		for (int i = 0; i < 4; i++) {
			float p = phase * mults[i] + phaseOffsets[i];
			p -= std::floor(p);

			float wave = evalWave(waves[i], p);     // [-1, 1]
			float out = wave * amplitude;           // bipolar, peak = amplitude

			outs[i]->setVoltage(out);

			// LED:
			float ledBrightness = clamp(std::fabs(out) / 5.f, 0.f, 1.f);
			leds[i]->setBrightnessSmooth(ledBrightness, args.sampleTime);
		}
	}
};

struct TL_LowFWidget : ModuleWidget {
	TL_LowFWidget(TL_LowF* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_LowF.svg")));

		// Screws
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// General controls
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(11.207, 25.205)), module, TL_LowF::AMP_KNOB_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(34.19, 25.176)), module, TL_LowF::FREQ_KNOB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.115, 43.668)), module, TL_LowF::AMP_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.313, 43.657)), module, TL_LowF::FREQ_CV_INPUT));

		// Multipliers
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(8.188, 78.412)), module, TL_LowF::MULTIPLO1_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(37.613, 78.436)), module, TL_LowF::MULTIPLO2_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(8.179, 109.092)), module, TL_LowF::MULTIPLO3_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(37.594, 109.159)), module, TL_LowF::MULTIPLO4_PARAM));
		
		// Waves
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(16.121, 76.85)), module, TL_LowF::ONDA1_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(29.661, 76.8)), module, TL_LowF::ONDA2_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(16.124, 107.551)), module, TL_LowF::ONDA3_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(29.651, 107.487)), module, TL_LowF::ONDA4_PARAM));
		
		// Outputs
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(10.242, 61.627)), module, TL_LowF::OUT1_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(35.47, 61.663)), module, TL_LowF::OUT2_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(8.658, 92.319)), module, TL_LowF::OUT3_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(37.063, 92.329)), module, TL_LowF::OUT4_OUTPUT));

		// Leds
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(5.24, 56.334)), module, TL_LowF::LED1_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(40.406, 56.31)), module, TL_LowF::LED2_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(3.43, 97.54)), module, TL_LowF::LED3_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(42.23, 97.515)), module, TL_LowF::LED4_LIGHT));
	}
};

Model* modelTL_LowF = createModel<TL_LowF, TL_LowFWidget>("TL_LowF");