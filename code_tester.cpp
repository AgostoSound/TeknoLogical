#include "plugin.hpp"


struct TL_Reseter : Module {
	enum ParamId {
		PUSH_A_PARAM,
		PUSH_B_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_A_INPUT,
		IN_B_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		PUSH_A_LED,
		PUSH_B_LED,
		LIGHTS_LEN
	};

	TL_Reseter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PUSH_A_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PUSH_B_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_A_INPUT, "");
		configInput(IN_B_INPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TL_ReseterWidget : ModuleWidget {
	TL_ReseterWidget(TL_Reseter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Reseter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(7.653, 50.344)), module, TL_Reseter::PUSH_A_PARAM, TL_Reseter::PUSH_A_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(7.629, 98.987)), module, TL_Reseter::PUSH_B_PARAM, TL_Reseter::PUSH_B_LED));


		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.724, 33.390)), module, TL_Reseter::IN_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.677, 82.057)), module, TL_Reseter::IN_B_INPUT));

	}
};


Model* modelTL_Reseter = createModel<TL_Reseter, TL_ReseterWidget>("TL_Reseter");