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

	TL_LowF() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// General controls
		configParam(AMP_KNOB_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FREQ_KNOB_PARAM, 0.f, 1.f, 0.f, "");

		// Multipliers
		configParam(MULTIPLO1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MULTIPLO2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MULTIPLO3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MULTIPLO4_PARAM, 0.f, 1.f, 0.f, "");

		// Waves
		configParam(ONDA1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ONDA2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ONDA3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ONDA4_PARAM, 0.f, 1.f, 0.f, "");

		// Inputs
		configInput(AMP_CV_INPUT, "");
		configInput(FREQ_CV_INPUT, "");

		// Outputs
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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