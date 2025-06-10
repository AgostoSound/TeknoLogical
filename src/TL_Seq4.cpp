#include "plugin.hpp"


// General structure.
struct TL_Seq4 : Module {
// --------------------   Visual components namespace  ---------------------------
	enum ParamId {
		LENGTH_1_PARAM,
		REVERSE_1_PARAM,

		STEP_A1_PARAM,
		STEP_A2_PARAM,
		STEP_A3_PARAM,
		STEP_A4_PARAM,
		STEP_A5_PARAM,
		STEP_A6_PARAM,
		STEP_A7_PARAM,
		STEP_A8_PARAM,

		LENGTH_2_PARAM,
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
		LENGTH_1_INPUT,
		REVERSE_1_INPUT,

		IN_STEP_2_INPUT,
		LENGTH_2_INPUT,
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

		STEP_A1_LED,
		STEP_A2_LED,
		STEP_A3_LED,
		STEP_A4_LED,
		STEP_A5_LED,
		STEP_A6_LED,
		STEP_A7_LED,
		STEP_A8_LED,

		MINILED_A1_LIGHT,
		MINILED_A2_LIGHT,
		MINILED_A3_LIGHT,
		MINILED_A4_LIGHT,
		MINILED_A5_LIGHT,
		MINILED_A6_LIGHT,
		MINILED_A7_LIGHT,
		MINILED_A8_LIGHT,

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
		
		STEP_B1_LED,
		STEP_B2_LED,
		STEP_B3_LED,
		STEP_B4_LED,
		STEP_B5_LED,
		STEP_B6_LED,
		STEP_B7_LED,
		STEP_B8_LED,
		STEP_B9_LED,
		STEP_B10_LED,
		STEP_B11_LED,
		STEP_B12_LED,
		STEP_B13_LED,
		STEP_B14_LED,
		STEP_B15_LED,
		STEP_B16_LED,

		MINILED_B1_LIGHT,
		MINILED_B2_LIGHT,
		MINILED_B3_LIGHT,
		MINILED_B4_LIGHT,
		MINILED_B5_LIGHT,
		MINILED_B6_LIGHT,
		MINILED_B7_LIGHT,
		MINILED_B8_LIGHT,
		MINILED_B9_LIGHT,
		MINILED_B10_LIGHT,
		MINILED_B11_LIGHT,
		MINILED_B12_LIGHT,
		MINILED_B13_LIGHT,
		MINILED_B14_LIGHT,
		MINILED_B15_LIGHT,
		MINILED_B16_LIGHT,

		LIGHTS_LEN
	};

// --------------------   Set initial values  ------------------------------------

	// Seq A.
	bool latch_a1 = false, latch_a2 = false, latch_a3 = false, latch_a4 = false;
	bool latch_a5 = false, latch_a6 = false, latch_a7 = false, latch_a8 = false;
	bool length_a = false, reverse_a = false, input_a = false;
	bool cv_len_a = false, cv_rev_a = false;

	// Seq B.
	bool latch_b1 = false, latch_b2 = false, latch_b3 = false, latch_b4 = false;
	bool latch_b5 = false, latch_b6 = false, latch_b7 = false, latch_b8 = false;
	bool latch_b9 = false, latch_b10 = false, latch_b11 = false, latch_b12 = false;
	bool latch_b13 = false, latch_b14 = false, latch_b15 = false, latch_b16 = false;
	bool length_b = false, reverse_b = false, input_b = false;
	bool cv_len_b = false, cv_rev_b = false;

// --------------------   Config module  -----------------------------------------
	TL_Seq4() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		static const std::vector<std::string> on_off_labels = {"OFF", "ON"};
		
		// Sequencer A.
		static const std::vector<std::string> steps_labels_a = {"4", "8"};
		configSwitch(LENGTH_1_PARAM, 0.f, 1.f, 0.f, "Steps", steps_labels_a);
		configSwitch(REVERSE_1_PARAM, 0.f, 1.f, 0.f, "Reverse", on_off_labels);
		configInput(IN_STEP_1_INPUT, "Trigger A");
		configInput(LENGTH_1_INPUT, "CV Steps");
		configInput(REVERSE_1_INPUT, "CV Reverse");
		
		configSwitch(STEP_A1_PARAM, 0.f, 1.f, 0.f, "1", on_off_labels);
		configSwitch(STEP_A2_PARAM, 0.f, 1.f, 0.f, "2", on_off_labels);
		configSwitch(STEP_A3_PARAM, 0.f, 1.f, 0.f, "3", on_off_labels);
		configSwitch(STEP_A4_PARAM, 0.f, 1.f, 0.f, "4", on_off_labels);
		configSwitch(STEP_A5_PARAM, 0.f, 1.f, 0.f, "5", on_off_labels);
		configSwitch(STEP_A6_PARAM, 0.f, 1.f, 0.f, "6", on_off_labels);
		configSwitch(STEP_A7_PARAM, 0.f, 1.f, 0.f, "7", on_off_labels);
		configSwitch(STEP_A8_PARAM, 0.f, 1.f, 0.f, "8", on_off_labels);
		
		configOutput(OUT_1_OUTPUT, "Seq A");
		
		// Sequencer B.
		static const std::vector<std::string> steps_labels_b = {"8", "16"};
		configSwitch(LENGTH_2_PARAM, 0.f, 1.f, 0.f, "Steps", steps_labels_b);
		configSwitch(REVERSE_2_PARAM, 0.f, 1.f, 0.f, "Reverse", on_off_labels);
		configInput(IN_STEP_2_INPUT, "Trigger B");
		configInput(LENGTH_2_INPUT, "CV Steps");
		configInput(REVERSE_2_INPUT, "CV Reverse");
		
		configSwitch(STEP_B1_PARAM, 0.f, 1.f, 0.f, "1", on_off_labels);
		configSwitch(STEP_B2_PARAM, 0.f, 1.f, 0.f, "2", on_off_labels);
		configSwitch(STEP_B3_PARAM, 0.f, 1.f, 0.f, "3", on_off_labels);
		configSwitch(STEP_B4_PARAM, 0.f, 1.f, 0.f, "4", on_off_labels);
		configSwitch(STEP_B5_PARAM, 0.f, 1.f, 0.f, "5", on_off_labels);
		configSwitch(STEP_B6_PARAM, 0.f, 1.f, 0.f, "6", on_off_labels);
		configSwitch(STEP_B7_PARAM, 0.f, 1.f, 0.f, "7", on_off_labels);
		configSwitch(STEP_B8_PARAM, 0.f, 1.f, 0.f, "8", on_off_labels);
		configSwitch(STEP_B9_PARAM, 0.f, 1.f, 0.f, "9", on_off_labels);
		configSwitch(STEP_B10_PARAM, 0.f, 1.f, 0.f, "10", on_off_labels);
		configSwitch(STEP_B11_PARAM, 0.f, 1.f, 0.f, "11", on_off_labels);
		configSwitch(STEP_B12_PARAM, 0.f, 1.f, 0.f, "12", on_off_labels);
		configSwitch(STEP_B13_PARAM, 0.f, 1.f, 0.f, "13", on_off_labels);
		configSwitch(STEP_B14_PARAM, 0.f, 1.f, 0.f, "14", on_off_labels);
		configSwitch(STEP_B15_PARAM, 0.f, 1.f, 0.f, "15", on_off_labels);
		configSwitch(STEP_B16_PARAM, 0.f, 1.f, 0.f, "16", on_off_labels);
			
		configOutput(OUT_2_OUTPUT, "Seq B");
	}

// --------------------   Functions  ---------------------------------------------

	// Set steps leds.
	bool setStepsLeds(Module* module, const bool stepParams[], int numSteps) {		
		const int stepLEDs[] = {
			STEP_A1_LED, STEP_A2_LED, STEP_A3_LED, STEP_A4_LED, STEP_A5_LED, STEP_A6_LED, STEP_A7_LED, STEP_A8_LED,
			STEP_B1_LED, STEP_B2_LED, STEP_B3_LED, STEP_B4_LED, STEP_B5_LED, STEP_B6_LED, STEP_B7_LED, STEP_B8_LED, 
			STEP_B9_LED, STEP_B10_LED, STEP_B11_LED, STEP_B12_LED, STEP_B13_LED, STEP_B14_LED, STEP_B15_LED, STEP_B16_LED,
		};

		for (int i = 0; i < numSteps; ++i) {
			module->lights[stepLEDs[i]].setBrightness(stepParams[i] ? 1.0f : 0.0f);
		}

        return true;
    }
	
	// Read and update all inputs values.
	void updateAllInputStates() {
		// Seq A.
		latch_a1  = params[STEP_A1_PARAM].getValue() == 1.0f;
		latch_a2  = params[STEP_A2_PARAM].getValue() == 1.0f;
		latch_a3  = params[STEP_A3_PARAM].getValue() == 1.0f;
		latch_a4  = params[STEP_A4_PARAM].getValue() == 1.0f;
		latch_a5  = params[STEP_A5_PARAM].getValue() == 1.0f;
		latch_a6  = params[STEP_A6_PARAM].getValue() == 1.0f;
		latch_a7  = params[STEP_A7_PARAM].getValue() == 1.0f;
		latch_a8  = params[STEP_A8_PARAM].getValue() == 1.0f;

		length_a  = params[LENGTH_1_PARAM].getValue() == 1.0f;
		reverse_a = params[REVERSE_1_PARAM].getValue() == 1.0f;
		input_a   = inputs[IN_STEP_1_INPUT].getVoltage() >= 1.0f;
		cv_len_a  = inputs[LENGTH_1_INPUT].getVoltage() >= 1.0f;
		cv_rev_a  = inputs[REVERSE_1_INPUT].getVoltage() >= 1.0f;

		// Seq B.
		latch_b1  = params[STEP_B1_PARAM].getValue() == 1.0f;
		latch_b2  = params[STEP_B2_PARAM].getValue() == 1.0f;
		latch_b3  = params[STEP_B3_PARAM].getValue() == 1.0f;
		latch_b4  = params[STEP_B4_PARAM].getValue() == 1.0f;
		latch_b5  = params[STEP_B5_PARAM].getValue() == 1.0f;
		latch_b6  = params[STEP_B6_PARAM].getValue() == 1.0f;
		latch_b7  = params[STEP_B7_PARAM].getValue() == 1.0f;
		latch_b8  = params[STEP_B8_PARAM].getValue() == 1.0f;
		latch_b9  = params[STEP_B9_PARAM].getValue() == 1.0f;
		latch_b10 = params[STEP_B10_PARAM].getValue() == 1.0f;
		latch_b11 = params[STEP_B11_PARAM].getValue() == 1.0f;
		latch_b12 = params[STEP_B12_PARAM].getValue() == 1.0f;
		latch_b13 = params[STEP_B13_PARAM].getValue() == 1.0f;
		latch_b14 = params[STEP_B14_PARAM].getValue() == 1.0f;
		latch_b15 = params[STEP_B15_PARAM].getValue() == 1.0f;
		latch_b16 = params[STEP_B16_PARAM].getValue() == 1.0f;

		length_b  = params[LENGTH_2_PARAM].getValue() == 1.0f;
		reverse_b = params[REVERSE_2_PARAM].getValue() == 1.0f;
		input_b   = inputs[IN_STEP_2_INPUT].getVoltage() >= 1.0f;
		cv_len_b  = inputs[LENGTH_2_INPUT].getVoltage() >= 1.0f;
		cv_rev_b  = inputs[REVERSE_2_INPUT].getVoltage() >= 1.0f;
	}

	// --------------------   Main cycle logic  --------------------------------------
	void process(const ProcessArgs& args) override {
		updateAllInputStates();  // Update inputs values.
		
		const bool latchStates[] = {
			latch_a1, latch_a2, latch_a3, latch_a4, latch_a5, latch_a6, latch_a7, latch_a8,
			latch_b1, latch_b2, latch_b3, latch_b4, latch_b5, latch_b6, latch_b7, latch_b8,
			latch_b9, latch_b10, latch_b11, latch_b12, latch_b13, latch_b14, latch_b15, latch_b16
		};
		setStepsLeds(this, latchStates, 24);  // Latch leds.
	}
};


// --------------------   Visual components  -------------------------------------
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
		addParam(createParamCentered<NKK>(mm2px(Vec(22.535, 33.906)), module, TL_Seq4::LENGTH_1_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(68.936, 33.827)), module, TL_Seq4::REVERSE_1_PARAM));
		// Steps.
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(17.159, 52.133)), module, TL_Seq4::STEP_A1_PARAM, TL_Seq4::STEP_A1_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(25.368, 51.069)), module, TL_Seq4::STEP_A2_PARAM, TL_Seq4::STEP_A2_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(33.518, 50.005)), module, TL_Seq4::STEP_A3_PARAM, TL_Seq4::STEP_A3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(41.679, 48.988)), module, TL_Seq4::STEP_A4_PARAM, TL_Seq4::STEP_A4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(49.906, 48.953)), module, TL_Seq4::STEP_A5_PARAM, TL_Seq4::STEP_A5_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(58.045, 50.028)), module, TL_Seq4::STEP_A6_PARAM, TL_Seq4::STEP_A6_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(66.218, 51.092)), module, TL_Seq4::STEP_A7_PARAM, TL_Seq4::STEP_A7_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(74.391, 52.121)), module, TL_Seq4::STEP_A8_PARAM, TL_Seq4::STEP_A8_LED));
		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.371, 18.447)), module, TL_Seq4::IN_STEP_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.973, 34.265)), module, TL_Seq4::LENGTH_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.588, 34.314)), module, TL_Seq4::REVERSE_1_INPUT));
		// Output.
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.937, 18.496)), module, TL_Seq4::OUT_1_OUTPUT));
		// LEDs.
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(45.788, 14.285)), module, TL_Seq4::LED_A1_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(54.965, 18.117)), module, TL_Seq4::LED_A2_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(58.743, 27.276)), module, TL_Seq4::LED_A3_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(54.946, 36.447)), module, TL_Seq4::LED_A4_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(45.777, 40.251)), module, TL_Seq4::LED_A5_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(36.612, 36.437)), module, TL_Seq4::LED_A6_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(32.79, 27.284)), module, TL_Seq4::LED_A7_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(36.612, 18.091)), module, TL_Seq4::LED_A8_LIGHT));
		// Mini-LEDs.
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(17.228, 47.927)), module, TL_Seq4::MINILED_A1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(25.414, 46.848)), module, TL_Seq4::MINILED_A2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(33.55, 45.891)), module, TL_Seq4::MINILED_A3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(41.716, 44.754)), module, TL_Seq4::MINILED_A4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(49.953, 44.78)), module, TL_Seq4::MINILED_A5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(58.081, 45.834)), module, TL_Seq4::MINILED_A6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(66.242, 46.905)), module, TL_Seq4::MINILED_A7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(74.403, 47.96)), module, TL_Seq4::MINILED_A8_LIGHT));


		// Sequencer B.

		// Params.
		addParam(createParamCentered<NKK>(mm2px(Vec(22.496, 84.135)), module, TL_Seq4::LENGTH_2_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(68.893, 84.16)), module, TL_Seq4::REVERSE_2_PARAM));
		// Steps.
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(13.984, 104.521)), module, TL_Seq4::STEP_B1_PARAM, TL_Seq4::STEP_B1_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(23.251, 102.927)), module, TL_Seq4::STEP_B2_PARAM, TL_Seq4::STEP_B2_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(32.459, 101.863)), module, TL_Seq4::STEP_B3_PARAM, TL_Seq4::STEP_B3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(41.15, 100.846)), module, TL_Seq4::STEP_B4_PARAM, TL_Seq4::STEP_B4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(50.435, 100.811)), module, TL_Seq4::STEP_B5_PARAM, TL_Seq4::STEP_B5_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(59.103, 101.887)), module, TL_Seq4::STEP_B6_PARAM, TL_Seq4::STEP_B6_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(68.335, 102.951)), module, TL_Seq4::STEP_B7_PARAM, TL_Seq4::STEP_B7_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(77.566, 104.509)), module, TL_Seq4::STEP_B8_PARAM, TL_Seq4::STEP_B8_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(13.984, 114.046)), module, TL_Seq4::STEP_B9_PARAM, TL_Seq4::STEP_B9_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(23.251, 112.452)), module, TL_Seq4::STEP_B10_PARAM, TL_Seq4::STEP_B10_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(32.459, 111.388)), module, TL_Seq4::STEP_B11_PARAM, TL_Seq4::STEP_B11_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(41.15, 110.371)), module, TL_Seq4::STEP_B12_PARAM, TL_Seq4::STEP_B12_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(50.435, 110.336)), module, TL_Seq4::STEP_B13_PARAM, TL_Seq4::STEP_B13_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(59.103, 111.412)), module, TL_Seq4::STEP_B14_PARAM, TL_Seq4::STEP_B14_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(68.335, 112.476)), module, TL_Seq4::STEP_B15_PARAM, TL_Seq4::STEP_B15_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(mm2px(Vec(77.566, 114.034)), module, TL_Seq4::STEP_B16_PARAM, TL_Seq4::STEP_B16_LED));

		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.412, 68.624)), module, TL_Seq4::IN_STEP_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.973, 84.537)), module, TL_Seq4::LENGTH_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.575, 84.548)), module, TL_Seq4::REVERSE_2_INPUT));
		// Output.
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.869, 68.816)), module, TL_Seq4::OUT_2_OUTPUT));
		// LEDs.
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(45.689, 56.925)), module, TL_Seq4::LED_B1_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(52.132, 58.218)), module, TL_Seq4::LED_B2_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(57.672, 61.911)), module, TL_Seq4::LED_B3_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(61.318, 67.392)), module, TL_Seq4::LED_B4_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(62.593, 73.891)), module, TL_Seq4::LED_B5_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(61.311, 80.329)), module, TL_Seq4::LED_B6_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(57.603, 85.805)), module, TL_Seq4::LED_B7_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(52.157, 89.486)), module, TL_Seq4::LED_B8_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(45.663, 90.796)), module, TL_Seq4::LED_B9_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(39.226, 89.504)), module, TL_Seq4::LED_B10_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(33.738, 85.81)), module, TL_Seq4::LED_B11_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(30.043, 80.349)), module, TL_Seq4::LED_B12_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(28.746, 73.871)), module, TL_Seq4::LED_B13_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(30.059, 67.37)), module, TL_Seq4::LED_B14_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(33.744, 61.878)), module, TL_Seq4::LED_B15_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(39.215, 58.251)), module, TL_Seq4::LED_B16_LIGHT));
		// Mini-LEDs.
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(14.053, 100.315)), module, TL_Seq4::MINILED_B1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.297, 98.707)), module, TL_Seq4::MINILED_B2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(32.492, 97.749)), module, TL_Seq4::MINILED_B3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(41.186, 96.613)), module, TL_Seq4::MINILED_B4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(50.482, 96.638)), module, TL_Seq4::MINILED_B5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(59.139, 97.693)), module, TL_Seq4::MINILED_B6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(68.359, 98.763)), module, TL_Seq4::MINILED_B7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(77.578, 100.347)), module, TL_Seq4::MINILED_B8_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(14.053, 109.84)), module, TL_Seq4::MINILED_B9_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.297, 108.232)), module, TL_Seq4::MINILED_B10_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(32.492, 107.274)), module, TL_Seq4::MINILED_B11_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(41.186, 106.138)), module, TL_Seq4::MINILED_B12_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(50.482, 106.163)), module, TL_Seq4::MINILED_B13_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(59.139, 107.218)), module, TL_Seq4::MINILED_B14_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(68.359, 108.288)), module, TL_Seq4::MINILED_B15_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(77.578, 109.872)), module, TL_Seq4::MINILED_B16_LIGHT));


	}
};


Model* modelTL_Seq4 = createModel<TL_Seq4, TL_Seq4Widget>("TL_Seq4");