#include "plugin.hpp"


struct TL_Bass : Module {
	enum ParamId {
		TRIGGER_LATCH_PARAM,
		FILTER_KNOB_PARAM,
		DECAY_KNOB_PARAM,
		TIMBRE_SELECTOR_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TRIGGER_JACK_INPUT,
		VOCT_JACK_INPUT,
		FILTER_JACK_INPUT,
		DECAY_JACK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_MONO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		TRIGGER_LATCH_LIGHT,
		LIGHTS_LEN
	};

	TL_Bass() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(TRIGGER_LATCH_PARAM, 0.f, 1.f, 0.f, "Trigger");
		configParam(FILTER_KNOB_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(DECAY_KNOB_PARAM, -10.f, 10.f, 0.f, "Decay");
		configSwitch(TIMBRE_SELECTOR_PARAM, 0.f, 1.f, 1.f, "Timbre", {"2", "1"});
		configInput(TRIGGER_JACK_INPUT, "Trigger");
		configInput(VOCT_JACK_INPUT, "V/Oct");
		configInput(FILTER_JACK_INPUT, "Filter");
		configInput(DECAY_JACK_INPUT, "Decay");
		configOutput(OUT_MONO_OUTPUT, "Mono");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TL_BassWidget : ModuleWidget {
	TL_BassWidget(TL_Bass* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Bass.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(15.142, 24.404)), module, TL_Bass::TRIGGER_LATCH_PARAM, TL_Bass::TRIGGER_LATCH_LIGHT));
		
		addParam(createParamCentered<CKSS>(mm2px(Vec(7.957, 109.690)), module, TL_Bass::TIMBRE_SELECTOR_PARAM));

		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(15.056, 46.994)), module, TL_Bass::FILTER_KNOB_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(14.933, 68.928)), module, TL_Bass::DECAY_KNOB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.297, 28.152)), module, TL_Bass::TRIGGER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.206, 28.152)), module, TL_Bass::VOCT_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.891, 92.414)), module, TL_Bass::FILTER_JACK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.65, 92.422)), module, TL_Bass::DECAY_JACK_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(22.638, 108.322)), module, TL_Bass::OUT_MONO_OUTPUT));
	}
};


Model* modelTL_Bass = createModel<TL_Bass, TL_BassWidget>("TL_Bass");