#include "plugin.hpp"
#include "../helpers/widgets/sliders.hpp"
#include "dsp/pump_envelope.hpp"

struct TL_Pump : Module {
	enum ParamId {
		SHAPE_PARAM,
		TRIGGER_MANUAL_PARAM,
		DRYWET_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TRIGGER_INPUT,
		IN_L_INPUT,
		IN_R_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED_A_LIGHT,
		LED_B_LIGHT,
		LED_C_LIGHT,
		LED_D_LIGHT,
		SHAPE_LED,
		TRIGGER_MANUAL_LED,
		LIGHTS_LEN
	};

	int shapeIndex = 0;
	dsp::SchmittTrigger shapeTrig;
	dsp::SchmittTrigger manualTrig;
	dsp::SchmittTrigger inputTrig;
	TeknoDSP::PumpEnvelope envelope;

	TL_Pump() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Shape");
		configParam(TRIGGER_MANUAL_PARAM, 0.f, 1.f, 0.f, "Trigger");
		configParam(DRYWET_PARAM, 0.f, 1.f, 1.f, "Dry/Wet");

		configInput(TRIGGER_INPUT, "Trigger");
		configInput(IN_L_INPUT, "In L");
		configInput(IN_R_INPUT, "In R");

		configOutput(OUT_L_OUTPUT, "Out L");
		configOutput(OUT_R_OUTPUT, "Out R");
	}

	void process(const ProcessArgs& args) override {
		if (shapeTrig.process(params[SHAPE_PARAM].getValue()))
			shapeIndex = (shapeIndex + 1) & 3;

		bool trig = false;
		if (manualTrig.process(params[TRIGGER_MANUAL_PARAM].getValue()))
			trig = true;
		if (inputs[TRIGGER_INPUT].isConnected() && inputTrig.process(inputs[TRIGGER_INPUT].getVoltage()))
			trig = true;

		if (trig)
			envelope.trigger();

		float gain = envelope.process(shapeIndex, args.sampleTime);

		float inL = inputs[IN_L_INPUT].getVoltage();
		float inR = inputs[IN_R_INPUT].isConnected() ? inputs[IN_R_INPUT].getVoltage() : inL;

		float wetL = inL * gain;
		float wetR = inR * gain;

		float dryWet = clamp(params[DRYWET_PARAM].getValue(), 0.f, 1.f);
		float outL = inL * (1.f - dryWet) + wetL * dryWet;
		float outR = inR * (1.f - dryWet) + wetR * dryWet;

		outputs[OUT_L_OUTPUT].setVoltage(outL);
		outputs[OUT_R_OUTPUT].setVoltage(outR);

		lights[LED_A_LIGHT].setBrightness(shapeIndex == 0 ? 1.f : 0.f);
		lights[LED_B_LIGHT].setBrightness(shapeIndex == 1 ? 1.f : 0.f);
		lights[LED_C_LIGHT].setBrightness(shapeIndex == 2 ? 1.f : 0.f);
		lights[LED_D_LIGHT].setBrightness(shapeIndex == 3 ? 1.f : 0.f);

		lights[SHAPE_LED].setBrightness(clamp(params[SHAPE_PARAM].getValue(), 0.f, 1.f));

		float pumpAmount = 1.f - clamp(gain, 0.f, 1.f);
		lights[TRIGGER_MANUAL_LED].setBrightness(clamp(pumpAmount, 0.f, 1.f));
	}
};

struct TL_PumpWidget : ModuleWidget {
	TL_PumpWidget(TL_Pump* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Pump.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(5.724, 31.038)), module, TL_Pump::SHAPE_PARAM, TL_Pump::SHAPE_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(24.749, 31.072)), module, TL_Pump::TRIGGER_MANUAL_PARAM, TL_Pump::TRIGGER_MANUAL_LED));
		addParam(createParamCentered<SmallHSliderBool>(mm2px(Vec(15.097, 110.059)), module, TL_Pump::DRYWET_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.096, 25.578)), module, TL_Pump::TRIGGER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.415, 81.202)), module, TL_Pump::IN_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.415, 92.827)), module, TL_Pump::IN_R_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(21.985, 81.188)), module, TL_Pump::OUT_L_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(21.976, 92.871)), module, TL_Pump::OUT_R_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.51, 42.014)), module, TL_Pump::LED_A_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(27.02, 42.004)), module, TL_Pump::LED_B_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(3.498, 68.468)), module, TL_Pump::LED_C_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(27.023, 68.479)), module, TL_Pump::LED_D_LIGHT));
	}
};

Model* modelTL_Pump = createModel<TL_Pump, TL_PumpWidget>("TL_Pump");
