#include "plugin.hpp"
#include "../helpers/widgets/sliders.hpp"


struct TL_Shape : Module {
	enum ParamId {
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		TRIGGER_PARAM,
		VOL_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		TRIG_CV_INPUT,
		CV_IN_INPUT,
		L_IN_INPUT,
		R_IN_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		CV_OUT_OUTPUT,
		L_OUT_OUTPUT,
		R_OUT_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		L_LED_1_LIGHT,
		R_LED_1_LIGHT,
		L_LED_2_LIGHT,
		R_LED_2_LIGHT,
		L_LED_3_LIGHT,
		R_LED_3_LIGHT,
		L_LED_4_LIGHT,
		R_LED_4_LIGHT,
		L_LED_5_LIGHT,
		R_LED_5_LIGHT,
		L_LED_6_LIGHT,
		R_LED_6_LIGHT,
		L_LED_7_LIGHT,
		R_LED_7_LIGHT,
		TRIGGER_PARAM_LED,
		LIGHTS_LEN
	};

	enum EnvStage {
		IDLE_STAGE,
		ATTACK_STAGE,
		DECAY_STAGE,
		SUSTAIN_STAGE,
		RELEASE_STAGE
	};

	EnvStage envStage = IDLE_STAGE;

	float env = 0.f;
	float releaseStart = 0.f;

	dsp::SchmittTrigger gateTrigger;

	TL_Shape() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(ATTACK_PARAM, 0.f, 1.f, 0.f, "Attack");
		configParam(DECAY_PARAM, 0.f, 1.f, 0.f, "Decay");
		configParam(SUSTAIN_PARAM, 0.f, 1.f, 0.f, "Sustain");
		configParam(RELEASE_PARAM, 0.f, 1.f, 0.f, "Release");
		configButton(TRIGGER_PARAM, "Trigger");
		configParam(VOL_PARAM, 0.f, 1.f, 0.f, "Volume");

		configInput(TRIG_CV_INPUT, "Trigger");
		configInput(CV_IN_INPUT, "CV");
		configInput(L_IN_INPUT, "Left audio");
		configInput(R_IN_INPUT, "Right audio");

		configOutput(CV_OUT_OUTPUT, "CV");
		configOutput(L_OUT_OUTPUT, "Left audio");
		configOutput(R_OUT_OUTPUT, "Right audio");
	}

	float knobToTime(float value, float minTime = 0.001f, float maxTime = 10.f) {
		return minTime * std::pow(maxTime / minTime, value);
	}

	void startRelease() {
		if (envStage != RELEASE_STAGE && envStage != IDLE_STAGE) {
			releaseStart = env;
			envStage = RELEASE_STAGE;
		}
	}

	void processEnvelope(const ProcessArgs& args) {
		const float attackTime = knobToTime(params[ATTACK_PARAM].getValue());
		const float decayTime = knobToTime(params[DECAY_PARAM].getValue());
		const float sustainLevel = params[SUSTAIN_PARAM].getValue();
		const float releaseTime = knobToTime(params[RELEASE_PARAM].getValue());

		const bool manualGate = params[TRIGGER_PARAM].getValue() > 0.5f;
		const bool inputGate = inputs[TRIG_CV_INPUT].isConnected() && inputs[TRIG_CV_INPUT].getVoltage() >= 1.f;
		const bool gate = manualGate || inputGate;

		if (gateTrigger.process(gate ? 10.f : 0.f)) {
			envStage = ATTACK_STAGE;
		}

		if (!gate && envStage != IDLE_STAGE) {
			startRelease();
		}

		switch (envStage) {
			case IDLE_STAGE: {
				env = 0.f;
			} break;

			case ATTACK_STAGE: {
				env += args.sampleTime / attackTime;

				if (env >= 1.f) {
					env = 1.f;
					envStage = DECAY_STAGE;
				}
			} break;

			case DECAY_STAGE: {
				env -= args.sampleTime * (1.f - sustainLevel) / decayTime;

				if (env <= sustainLevel) {
					env = sustainLevel;
					envStage = SUSTAIN_STAGE;
				}
			} break;

			case SUSTAIN_STAGE: {
				env = sustainLevel;
			} break;

			case RELEASE_STAGE: {
				env -= args.sampleTime * releaseStart / releaseTime;

				if (env <= 0.f) {
					env = 0.f;
					envStage = IDLE_STAGE;
				}
			} break;
		}

		env = clamp(env, 0.f, 1.f);

		outputs[CV_OUT_OUTPUT].setVoltage(env * 10.f);

		lights[TRIGGER_PARAM_LED].setBrightnessSmooth(gate ? 1.f : 0.f, args.sampleTime);
	}

	void processVca() {
		const float cv = inputs[CV_IN_INPUT].isConnected()
			? clamp(inputs[CV_IN_INPUT].getVoltage() / 10.f, 0.f, 1.f)
			: 0.f;

		const float volume = params[VOL_PARAM].getValue();
		const float gain = cv * volume;

		float left = 0.f;
		float right = 0.f;

		const bool leftConnected = inputs[L_IN_INPUT].isConnected();
		const bool rightConnected = inputs[R_IN_INPUT].isConnected();

		if (leftConnected && rightConnected) {
			left = inputs[L_IN_INPUT].getVoltage();
			right = inputs[R_IN_INPUT].getVoltage();
		}
		else if (leftConnected) {
			left = inputs[L_IN_INPUT].getVoltage();
			right = left;
		}
		else if (rightConnected) {
			right = inputs[R_IN_INPUT].getVoltage();
			left = right;
		}

		outputs[L_OUT_OUTPUT].setVoltage(left * gain);
		outputs[R_OUT_OUTPUT].setVoltage(right * gain);
	}

	void processLights(const ProcessArgs& args) {
		const float level = inputs[CV_IN_INPUT].isConnected()
			? clamp(inputs[CV_IN_INPUT].getVoltage() / 10.f, 0.f, 1.f)
			: env;

		for (int i = 0; i < 7; i++) {
			const float threshold = (float) (i + 1) / 7.f;
			const float brightness = level >= threshold ? 1.f : 0.f;

			lights[L_LED_1_LIGHT + i * 2].setBrightnessSmooth(brightness, args.sampleTime);
			lights[R_LED_1_LIGHT + i * 2].setBrightnessSmooth(brightness, args.sampleTime);
		}
	}

	void process(const ProcessArgs& args) override {
		processEnvelope(args);
		processVca();
		processLights(args);
	}
};


struct TL_ShapeWidget : ModuleWidget {
	TL_ShapeWidget(TL_Shape* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Shape.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// ADSR
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(5.064, 27.111)), module, TL_Shape::ATTACK_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(11.993, 25.013)), module, TL_Shape::DECAY_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(18.512, 24.941)), module, TL_Shape::SUSTAIN_PARAM));
		addParam(createParamCentered<MiniVSlider>(mm2px(Vec(25.379, 27.073)), module, TL_Shape::RELEASE_PARAM));

		// Trigger / CV envelope
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(15.253, 36.213)),
			module, TL_Shape::TRIGGER_PARAM, TL_Shape::TRIGGER_PARAM_LED));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.37, 46.928)), module, TL_Shape::TRIG_CV_INPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(15.308, 62.866)), module, TL_Shape::CV_OUT_OUTPUT));

		// CV VCA
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.348, 80.471)), module, TL_Shape::CV_IN_INPUT));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(22.11, 80.54)), module, TL_Shape::VOL_PARAM));

		// Audio I/O
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.301, 97.921)), module, TL_Shape::L_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.283, 108.331)), module, TL_Shape::R_IN_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(22.212, 97.94)), module, TL_Shape::L_OUT_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(22.23, 108.497)), module, TL_Shape::R_OUT_OUTPUT));

		// Envelope / VCA LEDs
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(4.046, 66.82)), module, TL_Shape::L_LED_1_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(26.249, 66.848)), module, TL_Shape::R_LED_1_LIGHT));

		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(4.037, 61.032)), module, TL_Shape::L_LED_2_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(26.256, 61.023)), module, TL_Shape::R_LED_2_LIGHT));

		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(3.677, 55.909)), module, TL_Shape::L_LED_3_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(26.495, 55.849)), module, TL_Shape::R_LED_3_LIGHT));

		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(3.526, 51.09)), module, TL_Shape::L_LED_4_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(26.727, 51.093)), module, TL_Shape::R_LED_4_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.299, 46.31)), module, TL_Shape::L_LED_5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(26.986, 46.238)), module, TL_Shape::R_LED_5_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.007, 42.546)), module, TL_Shape::L_LED_6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(27.219, 42.545)), module, TL_Shape::R_LED_6_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.01, 38.846)), module, TL_Shape::L_LED_7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(27.215, 38.835)), module, TL_Shape::R_LED_7_LIGHT));
	}
};


Model* modelTL_Shape = createModel<TL_Shape, TL_ShapeWidget>("TL_Shape");