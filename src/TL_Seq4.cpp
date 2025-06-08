#include "plugin.hpp"


struct TL_Seq4 : Module {
	enum ParamId {
		RANDOM_1_PARAM,
		REVERSE_1_PARAM,
		STEP_A1_PARAM,
		STEP_A2_PARAM,
		STEP_A3_PARAM,
		STEP_A4_PARAM,
		STEP_A5_PARAM,
		STEP_A6_PARAM,
		STEP_A7_PARAM,
		STEP_A8_PARAM,
		RANDOM_2_PARAM,
		REVERSE_2_PARAM,
		STEP_B1_PARAM,
		STEP_B2_PARAM,
		STEP_B3_PARAM,
		STEP_B4_PARAM,
		STEP_B5_PARAM,
		STEP_B6_PARAM,
		STEP_B7_PARAM,
		STEP_B8_PARAM,
		STEP_B9_PARAM,
		STEP_B10_PARAM,
		STEP_B11_PARAM,
		STEP_B12_PARAM,
		STEP_B13_PARAM,
		STEP_B14_PARAM,
		STEP_B15_PARAM,
		STEP_B16_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_STEP_1_INPUT,
		RANDOM_1_INPUT,
		REVERSE_1_INPUT,
		IN_STEP_2_INPUT,
		RANDOM_2_INPUT,
		REVERSE_2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED_A1_LIGHT,
		LED_A2_LIGHT,
		LED_A3_LIGHT,
		LED_A4_LIGHT,
		LED_A5_LIGHT,
		LED_A6_LIGHT,
		LED_A7_LIGHT,
		LED_A8_LIGHT,
		LED_B1_LIGHT,
		LED_B2_LIGHT,
		LED_B3_LIGHT,
		LED_B4_LIGHT,
		LED_B5_LIGHT,
		LED_B6_LIGHT,
		LED_B7_LIGHT,
		LED_B8_LIGHT,
		LED_B9_LIGHT,
		LED_B10_LIGHT,
		LED_B11_LIGHT,
		LED_B12_LIGHT,
		LED_B13_LIGHT,
		LED_B14_LIGHT,
		LED_B15_LIGHT,
		LED_B16_LIGHT,
		LIGHTS_LEN
	};
	
	TL_Seq4() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		// Sequencer A.
		configParam(RANDOM_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REVERSE_1_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_STEP_1_INPUT, "");
		configInput(RANDOM_1_INPUT, "");
		configInput(REVERSE_1_INPUT, "");
		
		configParam(STEP_A1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_A8_PARAM, 0.f, 1.f, 0.f, "");
		
		configOutput(OUT_1_OUTPUT, "");

		// Sequencer B.
		configParam(RANDOM_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REVERSE_2_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_STEP_2_INPUT, "");
		configInput(RANDOM_2_INPUT, "");
		configInput(REVERSE_2_INPUT, "");
		
		configParam(STEP_B1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B8_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B9_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B10_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B11_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B12_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B13_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B14_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B15_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_B16_PARAM, 0.f, 1.f, 0.f, "");
			
		configOutput(OUT_2_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TL_Seq4Widget : ModuleWidget {
	TL_Seq4Widget(TL_Seq4* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Seq4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Sequencer A.

		// Params.
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.535, 33.906)), module, TL_Seq4::RANDOM_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.936, 33.827)), module, TL_Seq4::REVERSE_1_PARAM));
		// Steps.
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.159, 52.133)), module, TL_Seq4::STEP_A1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.368, 51.069)), module, TL_Seq4::STEP_A2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.518, 50.005)), module, TL_Seq4::STEP_A3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.679, 48.988)), module, TL_Seq4::STEP_A4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(49.906, 48.953)), module, TL_Seq4::STEP_A5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(58.045, 50.028)), module, TL_Seq4::STEP_A6_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.218, 51.092)), module, TL_Seq4::STEP_A7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(74.391, 52.121)), module, TL_Seq4::STEP_A8_PARAM));
		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.371, 18.447)), module, TL_Seq4::IN_STEP_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.973, 34.265)), module, TL_Seq4::RANDOM_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.588, 34.314)), module, TL_Seq4::REVERSE_1_INPUT));
		// Output.
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(75.937, 18.496)), module, TL_Seq4::OUT_1_OUTPUT));
		// LEDs.
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.788, 14.285)), module, TL_Seq4::LED_A1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.965, 18.117)), module, TL_Seq4::LED_A2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(58.743, 27.276)), module, TL_Seq4::LED_A3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.946, 36.447)), module, TL_Seq4::LED_A4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.777, 40.251)), module, TL_Seq4::LED_A5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.612, 36.437)), module, TL_Seq4::LED_A6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(32.79, 27.284)), module, TL_Seq4::LED_A7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.612, 18.091)), module, TL_Seq4::LED_A8_LIGHT));


		// Sequencer B.

		// Params.
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.496, 84.135)), module, TL_Seq4::RANDOM_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.893, 84.16)), module, TL_Seq4::REVERSE_2_PARAM));
		// Steps.
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.984, 104.521)), module, TL_Seq4::STEP_B1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.251, 102.927)), module, TL_Seq4::STEP_B2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.459, 101.863)), module, TL_Seq4::STEP_B3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.15, 100.846)), module, TL_Seq4::STEP_B4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.435, 100.811)), module, TL_Seq4::STEP_B5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.103, 101.887)), module, TL_Seq4::STEP_B6_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.335, 102.951)), module, TL_Seq4::STEP_B7_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.566, 104.509)), module, TL_Seq4::STEP_B8_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.984, 114.046)), module, TL_Seq4::STEP_B9_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.251, 112.452)), module, TL_Seq4::STEP_B10_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.459, 111.388)), module, TL_Seq4::STEP_B11_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.15, 110.371)), module, TL_Seq4::STEP_B12_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.435, 110.336)), module, TL_Seq4::STEP_B13_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.103, 111.412)), module, TL_Seq4::STEP_B14_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.335, 112.476)), module, TL_Seq4::STEP_B15_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.566, 114.034)), module, TL_Seq4::STEP_B16_PARAM));
		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.412, 68.624)), module, TL_Seq4::IN_STEP_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.973, 84.537)), module, TL_Seq4::RANDOM_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.575, 84.548)), module, TL_Seq4::REVERSE_2_INPUT));
		// Output.
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(75.869, 68.816)), module, TL_Seq4::OUT_2_OUTPUT));
		// LEDs.
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.689, 56.925)), module, TL_Seq4::LED_B1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(52.132, 58.218)), module, TL_Seq4::LED_B2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.672, 61.911)), module, TL_Seq4::LED_B3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(61.318, 67.392)), module, TL_Seq4::LED_B4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(62.593, 73.891)), module, TL_Seq4::LED_B5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(61.311, 80.329)), module, TL_Seq4::LED_B6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(57.603, 85.805)), module, TL_Seq4::LED_B7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(52.157, 89.486)), module, TL_Seq4::LED_B8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.663, 90.796)), module, TL_Seq4::LED_B9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(39.226, 89.504)), module, TL_Seq4::LED_B10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(33.738, 85.81)), module, TL_Seq4::LED_B11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.043, 80.349)), module, TL_Seq4::LED_B12_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.746, 73.871)), module, TL_Seq4::LED_B13_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.059, 67.37)), module, TL_Seq4::LED_B14_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(33.744, 61.878)), module, TL_Seq4::LED_B15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(39.215, 58.251)), module, TL_Seq4::LED_B16_LIGHT));

	}
};


Model* modelTL_Seq4 = createModel<TL_Seq4, TL_Seq4Widget>("TL_Seq4");