#include "plugin.hpp"


struct TL_Deck : Module {
	enum ParamId {
		KNOB_VOL_PARAM,
		LATCH_LOAD_PARAM,
		LATCH_PLAY_PARAM,
		LATCH_STOP_PARAM,
		SCREEN_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		CV_PLAY_INPUT,
		CV_STOP_INPUT,
		CV_VOL_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		L_OUT_OUTPUT,
		R_OUT_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LED_LOAD_LIGHT,
		LED_PLAY_LIGHT,
		LED_STOP_LIGHT,
		LATCH_LOAD_LED,
		LATCH_PLAY_LED,
		LATCH_STOP_LED,
		LIGHTS_LEN
	};

	TL_Deck() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(KNOB_VOL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_LOAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_PLAY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_STOP_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCREEN_PARAM, 0.f, 1.f, 0.f, "");

		configInput(CV_PLAY_INPUT, "");
		configInput(CV_STOP_INPUT, "");
		configInput(CV_VOL_INPUT, "");

		configOutput(L_OUT_OUTPUT, "");
		configOutput(R_OUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TL_DeckWidget : ModuleWidget {
	TL_DeckWidget(TL_Deck* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Deck.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(22.498, 60.696)), module, TL_Deck::KNOB_VOL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.274, 36.399)), module, TL_Deck::SCREEN_PARAM));


		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(15.115, 83.198)), 
		module, TL_Deck::LATCH_LOAD_PARAM, TL_Deck::LATCH_LOAD_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(5.393, 86.504)), 
		module, TL_Deck::LATCH_PLAY_PARAM, TL_Deck::LATCH_PLAY_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(24.969, 86.091)), 
		module, TL_Deck::LATCH_STOP_PARAM, TL_Deck::LATCH_STOP_LED));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.049, 96.600)), module, TL_Deck::CV_PLAY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.599, 96.395)), module, TL_Deck::CV_STOP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.032, 60.631)), module, TL_Deck::CV_VOL_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(10.058, 109.231)), module, TL_Deck::L_OUT_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(20.653, 109.306)), module, TL_Deck::R_OUT_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(15.238, 88.970)), module, TL_Deck::LED_LOAD_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(5.416, 80.663)), module, TL_Deck::LED_PLAY_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(24.961, 80.235)), module, TL_Deck::LED_STOP_LIGHT));
	}
};


Model* modelTL_Deck = createModel<TL_Deck, TL_DeckWidget>("TL_Deck");