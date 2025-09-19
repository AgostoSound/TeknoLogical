#include "plugin.hpp"


struct TL_Mixes : Module {
	enum ParamId {
		CUT_4_PARAM,
		CUT_3_PARAM,
		CUT_5_PARAM,
		CUT_2_PARAM,
		CUT_6_PARAM,
		CUT_1_PARAM,
		CUT_7_PARAM,
		PAN_4_PARAM,
		PAN_3_PARAM,
		PAN_5_PARAM,
		PAN_2_PARAM,
		PAN_6_PARAM,
		PAN_1_PARAM,
		PAN_7_PARAM,
		VOL_4_PARAM,
		VOL_3_PARAM,
		VOL_5_PARAM,
		VOL_2_PARAM,
		VOL_6_PARAM,
		VOL_1_PARAM,
		VOL_7_PARAM,
		MUTE_4_PARAM,
		SOLO_4_PARAM,
		SOLO_3_PARAM,
		MUTE_5_PARAM,
		MUTE_3_PARAM,
		SOLO_5_PARAM,
		SOLO_2_PARAM,
		MUTE_6_PARAM,
		MUTE_2_PARAM,
		SOLO_6_PARAM,
		SOLO_1_PARAM,
		MUTE_1_PARAM,
		MUTE_7_PARAM,
		SOLO_7_PARAM,
		MASTER_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		L_IN_4_INPUT,
		L_IN_3_INPUT,
		L_IN_5_INPUT,
		L_IN_2_INPUT,
		L_IN_6_INPUT,
		L_IN_1_INPUT,
		L_IN_7_INPUT,
		R_IN_4_INPUT,
		R_IN_3_INPUT,
		R_IN_5_INPUT,
		R_IN_2_INPUT,
		R_IN_6_INPUT,
		R_IN_1_INPUT,
		R_IN_7_INPUT,
		VOL_IN_4_INPUT,
		VOL_IN_3_INPUT,
		VOL_IN_5_INPUT,
		VOL_IN_2_INPUT,
		VOL_IN_6_INPUT,
		VOL_IN_1_INPUT,
		VOL_IN_7_INPUT,
		PAN_IN_4_INPUT,
		PAN_IN_3_INPUT,
		PAN_IN_5_INPUT,
		PAN_IN_2_INPUT,
		PAN_IN_6_INPUT,
		PAN_IN_1_INPUT,
		PAN_IN_7_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED_4_LIGHT,
		LED_3_LIGHT,
		LED_5_LIGHT,
		LED_2_LIGHT,
		LED_6_LIGHT,
		LED_1_LIGHT,
		LED_7_LIGHT,
		L_VU_5_LIGHT,
		R_VU_5_LIGHT,
		L_VU_4_LIGHT,
		R_VU_4_LIGHT,
		L_VU_3_LIGHT,
		R_VU_3_LIGHT,
		L_VU_2_LIGHT,
		R_VU_2_LIGHT,
		L_VU_1_LIGHT,
		R_VU_1_LIGHT,
		MUTE_1_LED, MUTE_2_LED, MUTE_3_LED, MUTE_4_LED, MUTE_5_LED, MUTE_6_LED, MUTE_7_LED,
		SOLO_1_LED, SOLO_2_LED, SOLO_3_LED, SOLO_4_LED, SOLO_5_LED, SOLO_6_LED, SOLO_7_LED,
		LIGHTS_LEN
	};

	TL_Mixes() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		static const std::vector<std::string> onoff_labels = {"Off", "On"};

		configSwitch(CUT_1_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_2_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_3_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_4_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_5_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_6_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);
		configSwitch(CUT_7_PARAM, 0.f, 1.f, 0.f, "Cutoff", onoff_labels);

		configParam(PAN_1_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_2_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_3_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_4_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_5_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_6_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_7_PARAM, -1.f, 1.f, 0.f, "Pan");

		configParam(VOL_1_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_2_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_3_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_4_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_5_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_6_PARAM, 0.f, 10.f, 0.f, "Vol");
		configParam(VOL_7_PARAM, 0.f, 10.f, 0.f, "Vol");

		configSwitch(MUTE_1_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_2_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_3_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_4_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_5_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_6_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		configSwitch(MUTE_7_PARAM, 0.f, 1.f, 0.f, "Mute", onoff_labels);
		
		configSwitch(SOLO_1_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_2_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_3_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_4_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_5_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_6_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);
		configSwitch(SOLO_7_PARAM, 0.f, 1.f, 0.f, "Solo", onoff_labels);

		configParam(MASTER_PARAM, 0.f, 1.f, 0.f, "Master");

		configInput(L_IN_1_INPUT, "L");
		configInput(L_IN_2_INPUT, "L");
		configInput(L_IN_3_INPUT, "L");
		configInput(L_IN_4_INPUT, "L");
		configInput(L_IN_5_INPUT, "L");
		configInput(L_IN_6_INPUT, "L");
		configInput(L_IN_7_INPUT, "L");

		configInput(R_IN_1_INPUT, "R");
		configInput(R_IN_2_INPUT, "R");
		configInput(R_IN_3_INPUT, "R");
		configInput(R_IN_4_INPUT, "R");
		configInput(R_IN_5_INPUT, "R");
		configInput(R_IN_6_INPUT, "R");
		configInput(R_IN_7_INPUT, "R");

		configInput(VOL_IN_1_INPUT, "Vol");
		configInput(VOL_IN_2_INPUT, "Vol");
		configInput(VOL_IN_3_INPUT, "Vol");
		configInput(VOL_IN_4_INPUT, "Vol");
		configInput(VOL_IN_5_INPUT, "Vol");
		configInput(VOL_IN_6_INPUT, "Vol");
		configInput(VOL_IN_7_INPUT, "Vol");

		configInput(PAN_IN_1_INPUT, "Pan");
		configInput(PAN_IN_2_INPUT, "Pan");
		configInput(PAN_IN_3_INPUT, "Pan");
		configInput(PAN_IN_4_INPUT, "Pan");
		configInput(PAN_IN_5_INPUT, "Pan");
		configInput(PAN_IN_6_INPUT, "Pan");
		configInput(PAN_IN_7_INPUT, "Pan");

		configOutput(OUT_L_OUTPUT, "Left");
		configOutput(OUT_R_OUTPUT, "Right");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TL_MixesWidget : ModuleWidget {
	TL_MixesWidget(TL_Mixes* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Mixes.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<NKK>(mm2px(Vec(8.697, 64.656)), module, TL_Mixes::CUT_1_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(25.869, 62.004)), module, TL_Mixes::CUT_2_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(43.946, 59.346)), module, TL_Mixes::CUT_3_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(61.087, 57.221)), module, TL_Mixes::CUT_4_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(78.701, 59.284)), module, TL_Mixes::CUT_5_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(96.012, 61.92)), module, TL_Mixes::CUT_6_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(113.655, 64.528)), module, TL_Mixes::CUT_7_PARAM));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(8.666, 78.383)), module, TL_Mixes::PAN_1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(25.838, 75.731)), module, TL_Mixes::PAN_2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(43.915, 73.073)), module, TL_Mixes::PAN_3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(61.056, 70.948)), module, TL_Mixes::PAN_4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(78.67, 73.011)), module, TL_Mixes::PAN_5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(95.981, 75.647)), module, TL_Mixes::PAN_6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(113.624, 78.256)), module, TL_Mixes::PAN_7_PARAM));

		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(8.635, 91.752)), module, TL_Mixes::VOL_1_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(25.807, 89.1)), module, TL_Mixes::VOL_2_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(43.884, 86.442)), module, TL_Mixes::VOL_3_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(61.025, 84.317)), module, TL_Mixes::VOL_4_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(78.639, 86.38)), module, TL_Mixes::VOL_5_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(95.95, 89.016)), module, TL_Mixes::VOL_6_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(113.593, 91.624)), module, TL_Mixes::VOL_7_PARAM));
		
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(5.015, 104.696)), module, TL_Mixes::MUTE_1_PARAM, TL_Mixes::MUTE_1_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(22.529, 102.446)), module, TL_Mixes::MUTE_2_PARAM, TL_Mixes::MUTE_2_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(39.939, 99.866)), module, TL_Mixes::MUTE_3_PARAM, TL_Mixes::MUTE_3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(57.31, 97.269)), module, TL_Mixes::MUTE_4_PARAM, TL_Mixes::MUTE_4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(74.869, 98.905)), module, TL_Mixes::MUTE_5_PARAM, TL_Mixes::MUTE_5_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(92.377, 101.544)), module, TL_Mixes::MUTE_6_PARAM, TL_Mixes::MUTE_6_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(109.274, 104.159)), module, TL_Mixes::MUTE_7_PARAM, TL_Mixes::MUTE_7_LED));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(12.935, 103.665)), module, TL_Mixes::SOLO_1_PARAM, TL_Mixes::SOLO_1_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(30.449, 101.415)), module, TL_Mixes::SOLO_2_PARAM, TL_Mixes::SOLO_2_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(47.936, 98.866)), module, TL_Mixes::SOLO_3_PARAM, TL_Mixes::SOLO_3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(65.281, 97.254)), module, TL_Mixes::SOLO_4_PARAM, TL_Mixes::SOLO_4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(82.822, 99.89)), module, TL_Mixes::SOLO_5_PARAM, TL_Mixes::SOLO_5_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(100.297, 102.544)), module, TL_Mixes::SOLO_6_PARAM, TL_Mixes::SOLO_6_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(117.257, 105.27)), module, TL_Mixes::SOLO_7_PARAM, TL_Mixes::SOLO_7_LED));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(61.085, 107.963)), module, TL_Mixes::MASTER_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.565, 22.44)), module, TL_Mixes::L_IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.738, 19.788)), module, TL_Mixes::L_IN_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.814, 17.13)), module, TL_Mixes::L_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.956, 15.005)), module, TL_Mixes::L_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(78.569, 17.068)), module, TL_Mixes::L_IN_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(95.88, 19.704)), module, TL_Mixes::L_IN_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(113.524, 22.312)), module, TL_Mixes::L_IN_7_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.565, 31.987)), module, TL_Mixes::R_IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.738, 29.335)), module, TL_Mixes::R_IN_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.814, 26.677)), module, TL_Mixes::R_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.956, 24.552)), module, TL_Mixes::R_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(78.569, 26.615)), module, TL_Mixes::R_IN_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(95.88, 29.251)), module, TL_Mixes::R_IN_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(113.524, 31.859)), module, TL_Mixes::R_IN_7_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.576, 43.585)), module, TL_Mixes::VOL_IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.749, 40.932)), module, TL_Mixes::VOL_IN_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.825, 38.275)), module, TL_Mixes::VOL_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.967, 36.149)), module, TL_Mixes::VOL_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(78.58, 38.213)), module, TL_Mixes::VOL_IN_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(95.891, 40.848)), module, TL_Mixes::VOL_IN_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(113.534, 43.457)), module, TL_Mixes::VOL_IN_7_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.554, 53.11)), module, TL_Mixes::PAN_IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.727, 50.457)), module, TL_Mixes::PAN_IN_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.803, 47.8)), module, TL_Mixes::PAN_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.945, 45.674)), module, TL_Mixes::PAN_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(78.558, 47.738)), module, TL_Mixes::PAN_IN_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(95.869, 50.373)), module, TL_Mixes::PAN_IN_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(113.513, 52.982)), module, TL_Mixes::PAN_IN_7_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(55.805, 116.52)), module, TL_Mixes::OUT_L_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(66.731, 116.551)), module, TL_Mixes::OUT_R_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.48, 61.582)), module, TL_Mixes::LED_1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(20.652, 58.929)), module, TL_Mixes::LED_2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.729, 56.271)), module, TL_Mixes::LED_3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(55.87, 54.146)), module, TL_Mixes::LED_4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(73.483, 56.209)), module, TL_Mixes::LED_5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(90.795, 58.845)), module, TL_Mixes::LED_6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(108.438, 61.454)), module, TL_Mixes::LED_7_LIGHT));

		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(44.653, 121.596)), module, TL_Mixes::L_VU_1_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(44.666, 119.364)), module, TL_Mixes::L_VU_2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(44.659, 116.826)), module, TL_Mixes::L_VU_3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(44.674, 113.645)), module, TL_Mixes::L_VU_4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(44.674, 109.922)), module, TL_Mixes::L_VU_5_LIGHT));
		
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(78.233, 121.58)), module, TL_Mixes::R_VU_1_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(Vec(78.246, 119.349)), module, TL_Mixes::R_VU_2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(78.239, 116.81)), module, TL_Mixes::R_VU_3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(78.254, 113.63)), module, TL_Mixes::R_VU_4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(78.254, 109.907)), module, TL_Mixes::R_VU_5_LIGHT));
	}
};


Model* modelTL_Mixes = createModel<TL_Mixes, TL_MixesWidget>("TL_Mixes");