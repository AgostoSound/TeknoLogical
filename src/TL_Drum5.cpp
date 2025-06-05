#include <iostream>
#include <tuple>
#include "plugin.hpp"
#include "rack.hpp"
#include "dsp/filter.hpp"
#include "dsp_utils.hpp"
#include "../res/samples/kick.h"
#include "../res/samples/snare.h"
#include "../res/samples/clap.h"
#include "../res/samples/openhat.h"
#include "../res/samples/closedhat.h"
using namespace rack;


// General structure.
struct TL_Drum5 : Module {
// --------------------   Visual components namespace  ---------------------------
	enum ParamId {
		PUSH_KK_PARAM,
		LINK_KK_PARAM,
		LINK_OH_PARAM,
		PUSH_CH_PARAM,
		PUSH_OH_PARAM,
		LINK_CH_PARAM,
		LINK_SN_PARAM,
		PUSH_CL_PARAM,
		PUSH_SN_PARAM,
		LINK_CL_PARAM,
		PAN_KK_PARAM,
		VOL_KK_PARAM,
		VOL_OH_PARAM,
		PAN_CH_PARAM,
		PAN_OH_PARAM,
		VOL_CH_PARAM,
		VOL_SN_PARAM,
		PAN_CL_PARAM,
		PAN_SN_PARAM,
		VOL_CL_PARAM,
		DECAY_KK_PARAM,
		DECAY_OH_PARAM,
		DECAY_CH_PARAM,
		DECAY_SN_PARAM,
		DECAY_CL_PARAM,
		FILTER_KK_PARAM,
		FILTER_OH_PARAM,
		FILTER_CH_PARAM,
		FILTER_SN_PARAM,
		FILTER_CL_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_KK_INPUT,
		IN_CH_INPUT,
		IN_OH_INPUT,
		IN_CL_INPUT,
		IN_SN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_KK_OUTPUT,
		OUT_OH_OUTPUT,
		OUT_CH_OUTPUT,
		OUT_SN_OUTPUT,
		OUT_CL_OUTPUT,
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED_KK_LIGHT,
		LED_OH_LIGHT,
		LED_CH_LIGHT,
		LED_SN_LIGHT,
		LED_CL_LIGHT,
		LIGHTS_LEN
	};

// --------------------   Set initial values  ------------------------------------
    dsp::SchmittTrigger trigKick, trigClap, trigSnare, trigClosedHat, trigOpenHat;

	struct Voice {
        const int16_t* sample = nullptr;
        int length = 0;
        int pos = 0;
        bool playing = false;

        float step() {
            if (!playing || pos >= length)
                return 0.f;

            float out = (float)sample[pos++] / 32768.f;
            if (pos >= length)
                playing = false;
            return out;
        }

        void trigger(const int16_t* s, int len) {
            sample = s;
            length = len;
            pos = 0;
            playing = true;
        }
    };

    Voice kick, clap, snare, closedHat, openHat;

	DSPUtils::LowPassFilter kickLowFilter, clapLowFilter, snareLowFilter, closedHatLowFilter, openHatLowFilter;
	DSPUtils::HighPassFilter kickHighFilter, clapHighFilter, snareHighFilter, closedHatHighFilter, openHatHighFilter;
	DSPUtils::DecayEnvelope kickEnvelope, clapEnvelope, snareEnvelope, closedHatEnvelope, openHatEnvelope;

// --------------------   Config module  -----------------------------------------
	TL_Drum5() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		// PUSH.
		configParam(PUSH_CL_PARAM, 0.f, 1.f, 0.f, "Push");
		configParam(PUSH_CH_PARAM, 0.f, 1.f, 0.f, "Push");
		configParam(PUSH_KK_PARAM, 0.f, 1.f, 0.f, "Push");
		configParam(PUSH_OH_PARAM, 0.f, 1.f, 0.f, "Push");
		configParam(PUSH_SN_PARAM, 0.f, 1.f, 0.f, "Push");

		// LINK.
		configParam(LINK_CL_PARAM, 0.f, 1.f, 0.f, "Link");
		configParam(LINK_CH_PARAM, 0.f, 1.f, 0.f, "Link");
		configParam(LINK_KK_PARAM, 0.f, 1.f, 0.f, "Link");
		configParam(LINK_OH_PARAM, 0.f, 1.f, 0.f, "Link");
		configParam(LINK_SN_PARAM, 0.f, 1.f, 0.f, "Link");

		// PAN.
		configParam(PAN_CL_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_CH_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_KK_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_OH_PARAM, -1.f, 1.f, 0.f, "Pan");
		configParam(PAN_SN_PARAM, -1.f, 1.f, 0.f, "Pan");

		// VOL.
		configParam(VOL_CL_PARAM, 0.f, 10.f, 5.f, "Vol");
		configParam(VOL_CH_PARAM, 0.f, 10.f, 5.f, "Vol");
		configParam(VOL_KK_PARAM, 0.f, 10.f, 5.f, "Vol");
		configParam(VOL_OH_PARAM, 0.f, 10.f, 5.f, "Vol");
		configParam(VOL_SN_PARAM, 0.f, 10.f, 5.f, "Vol");

		// DECAY.
		configParam(DECAY_CL_PARAM, -10.f, 10.f, 0.f, "Decay");
		configParam(DECAY_CH_PARAM, -10.f, 10.f, 0.f, "Decay");
		configParam(DECAY_KK_PARAM, -10.f, 10.f, 0.f, "Decay");
		configParam(DECAY_OH_PARAM, -10.f, 10.f, 0.f, "Decay");
		configParam(DECAY_SN_PARAM, -10.f, 10.f, 0.f, "Decay");

		// FILTER.
		configParam(FILTER_CL_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(FILTER_CH_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(FILTER_KK_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(FILTER_OH_PARAM, -10.f, 10.f, 0.f, "Filter");
		configParam(FILTER_SN_PARAM, -10.f, 10.f, 0.f, "Filter");
		
		// TRIGGER INPUTS.
		configInput(IN_CL_INPUT, "Trigger clap");
		configInput(IN_CH_INPUT, "Trigger closed hat");
		configInput(IN_KK_INPUT, "Trigger kick");
		configInput(IN_OH_INPUT, "Trigger open hat");
		configInput(IN_SN_INPUT, "Trigger snare");
		
		// INDIVIDUAL OUTPUTS.
		configOutput(OUT_CL_OUTPUT, "Clap");
		configOutput(OUT_CH_OUTPUT, "Closed hat");
		configOutput(OUT_KK_OUTPUT, "Kick");
		configOutput(OUT_OH_OUTPUT, "Open hat");
		configOutput(OUT_SN_OUTPUT, "Snare");
		
		// STEREO OUTPUTS.
		configOutput(OUT_L_OUTPUT, "L");
		configOutput(OUT_R_OUTPUT, "R");
	}

// --------------------   Functions  ---------------------------------------------

	// Process per channel.
	std::tuple<float, float> processChannel(float trigger_in, float vol_in, float push_in, float filter_in, float decay_in, 
										float link_in, dsp::SchmittTrigger& schmittTrig, Voice& voice, DSPUtils::DecayEnvelope& envelope,
										float sampleRate, DSPUtils::LowPassFilter& lowFilter, DSPUtils::HighPassFilter& highFilter, 
										float individual_out, float led_out, float pan_in, float mixLeft, float mixRight, 
										const int16_t* sample_data, int sample_len) {		
		// Get input values.
		float trigger = inputs[trigger_in].getVoltage();
		float volume = params[vol_in].getValue();
		float push = params[push_in].getValue();
		float filter = params[filter_in].getValue();
		float decay = params[decay_in].getValue();
		float link = params[link_in].getValue();
		float pan = params[pan_in].getValue();
		
		// Trigger plays samples.
        if (schmittTrig.process(trigger)) 
		{voice.trigger(sample_data, sample_len); envelope.trigger(decay, sampleRate);}

		// Sample processors.
		float sample = voice.step();
		sample *= envelope.process(); 
		sample = DSPUtils::applyBoost(sample, push);
		sample = DSPUtils::applyLowPassFilter(sample, filter, sampleRate, lowFilter);
		sample = DSPUtils::applyHighPassFilter(sample, filter, sampleRate, highFilter);
		sample = DSPUtils::applyVolume(sample, volume);		
		
		outputs[individual_out].setVoltage(sample * 5.f);
		lights[led_out].setBrightness(std::fabs(sample));
		if (link == 0.f) 
		{
			float masterLeft, masterRight;
			DSPUtils::applyPan(sample, pan, masterLeft, masterRight);
			mixLeft += masterLeft;
			mixRight += masterRight;
		}

        return std::make_tuple(mixLeft, mixRight);
    }


// --------------------   Main cycle logic  --------------------------------------
	void process(const ProcessArgs& args) override {
		float sampleRate = args.sampleRate;

		// Variables para mezcla estéreo
		float mixLeft = 0.f;
		float mixRight = 0.f;

		std::tie(mixLeft, mixRight) = processChannel(IN_KK_INPUT, VOL_KK_PARAM, PUSH_KK_PARAM, FILTER_KK_PARAM, DECAY_KK_PARAM, 
			LINK_KK_PARAM, trigKick, kick, kickEnvelope, sampleRate, kickLowFilter, kickHighFilter, OUT_KK_OUTPUT, LED_KK_LIGHT, 
			PAN_KK_PARAM, mixLeft, mixRight, kick_sample, kick_sample_len);

		std::tie(mixLeft, mixRight) = processChannel(IN_SN_INPUT, VOL_SN_PARAM, PUSH_SN_PARAM, FILTER_SN_PARAM, DECAY_SN_PARAM, 
			LINK_SN_PARAM, trigSnare, snare, snareEnvelope, sampleRate, snareLowFilter, snareHighFilter, OUT_SN_OUTPUT, LED_SN_LIGHT, 
			PAN_SN_PARAM, mixLeft, mixRight, snare_sample, snare_sample_len);

		std::tie(mixLeft, mixRight) = processChannel(IN_CL_INPUT, VOL_CL_PARAM, PUSH_CL_PARAM, FILTER_CL_PARAM, DECAY_CL_PARAM, 
			LINK_CL_PARAM, trigClap, clap, clapEnvelope, sampleRate, clapLowFilter, clapHighFilter, OUT_CL_OUTPUT, LED_CL_LIGHT, 
			PAN_CL_PARAM, mixLeft, mixRight, clap_sample, clap_sample_len);

		std::tie(mixLeft, mixRight) = processChannel(IN_CH_INPUT, VOL_CH_PARAM, PUSH_CH_PARAM, FILTER_CH_PARAM, DECAY_CH_PARAM, 
			LINK_CH_PARAM, trigClosedHat, closedHat, closedHatEnvelope, sampleRate, closedHatLowFilter, closedHatHighFilter, 
			OUT_CH_OUTPUT, LED_CH_LIGHT, PAN_CH_PARAM, mixLeft, mixRight, closedhat_sample, closedhat_sample_len);

		std::tie(mixLeft, mixRight) = processChannel(IN_OH_INPUT, VOL_OH_PARAM, PUSH_OH_PARAM, FILTER_OH_PARAM, DECAY_OH_PARAM, 
			LINK_OH_PARAM, trigOpenHat, openHat, openHatEnvelope, sampleRate, openHatLowFilter, openHatHighFilter, OUT_OH_OUTPUT, 
			LED_OH_LIGHT, PAN_OH_PARAM, mixLeft, mixRight, openhat_sample, openhat_sample_len);

        // Salidas estéreo, escalado a ±5V
		outputs[OUT_L_OUTPUT].setVoltage(mixLeft * 5.f);
		outputs[OUT_R_OUTPUT].setVoltage(mixRight * 5.f)

	}
};


// --------------------   Visual components  -------------------------------------
struct TL_Drum5Widget : ModuleWidget {
	TL_Drum5Widget(TL_Drum5* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Drum5.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// PUSH.
		addParam(createParamCentered<NKK>(mm2px(Vec(105.35, 34.874)), module, TL_Drum5::PUSH_CL_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(81.367, 30.61)), module, TL_Drum5::PUSH_CH_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(56.698, 28.593)), module, TL_Drum5::PUSH_KK_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(31.734, 32.633)), module, TL_Drum5::PUSH_OH_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(6.964, 37.015)), module, TL_Drum5::PUSH_SN_PARAM));

		// LINK.
		addParam(createParamCentered<NKK>(mm2px(Vec(116.463, 36.96)), module, TL_Drum5::LINK_CL_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(92.502, 32.702)), module, TL_Drum5::LINK_CH_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(67.794, 28.5)), module, TL_Drum5::LINK_KK_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(42.932, 30.532)), module, TL_Drum5::LINK_OH_PARAM));
		addParam(createParamCentered<NKK>(mm2px(Vec(18.069, 34.85)), module, TL_Drum5::LINK_SN_PARAM));

		// PAN.
		addParam(createParamCentered<Trimpot>(mm2px(Vec(105.467, 52.009)), module, TL_Drum5::PAN_CL_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(81.484, 47.815)), module, TL_Drum5::PAN_CH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(56.698, 45.752)), module, TL_Drum5::PAN_KK_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(31.734, 49.838)), module, TL_Drum5::PAN_OH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(6.52, 53.963)), module, TL_Drum5::PAN_SN_PARAM));

		// VOL.
		addParam(createParamCentered<Trimpot>(mm2px(Vec(116.089, 54.165)), module, TL_Drum5::VOL_CL_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(92.292, 49.744)), module, TL_Drum5::VOL_CH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(67.794, 45.658)), module, TL_Drum5::VOL_KK_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(42.511, 47.713)), module, TL_Drum5::VOL_OH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(17.648, 51.775)), module, TL_Drum5::VOL_SN_PARAM));

		// DECAY.
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(110.584, 72.303)), module, TL_Drum5::DECAY_CL_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(86.766, 67.481)), module, TL_Drum5::DECAY_CH_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(61.903, 63.757)), module, TL_Drum5::DECAY_KK_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(36.906, 67.403)), module, TL_Drum5::DECAY_OH_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(12.078, 72.177)), module, TL_Drum5::DECAY_SN_PARAM));

		// FILTER.
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(110.531, 92.882)), module, TL_Drum5::FILTER_CL_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(86.766, 88.198)), module, TL_Drum5::FILTER_CH_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(61.874, 84.442)), module, TL_Drum5::FILTER_KK_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(36.965, 88.202)), module, TL_Drum5::FILTER_OH_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(12.189, 92.903)), module, TL_Drum5::FILTER_SN_PARAM));
		
		// TRIGGER INPUTS.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(105.58, 20.703)), module, TL_Drum5::IN_CL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(81.555, 16.503)), module, TL_Drum5::IN_CH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.699, 14.414)), module, TL_Drum5::IN_KK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.768, 18.678)), module, TL_Drum5::IN_OH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.62, 22.822)), module, TL_Drum5::IN_SN_INPUT));
		
		// INDIVIDUAL OUTPUTS.
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(115.608, 22.808)), module, TL_Drum5::OUT_CL_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(92.302, 18.603)), module, TL_Drum5::OUT_CH_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(67.618, 14.357)), module, TL_Drum5::OUT_KK_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(42.523, 16.495)), module, TL_Drum5::OUT_OH_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(17.66, 20.723)), module, TL_Drum5::OUT_SN_OUTPUT));
		
		// STEREO OUTPUTS.
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(56.615, 113.84)), module, TL_Drum5::OUT_L_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(67.63, 113.91)), module, TL_Drum5::OUT_R_OUTPUT));
		
		// LEDS.
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(73.667, 104.788)), module, TL_Drum5::LED_CL_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(67.832, 103.221)), module, TL_Drum5::LED_CH_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(62.032, 102.168)), module, TL_Drum5::LED_KK_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(56.209, 103.174)), module, TL_Drum5::LED_OH_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(50.377, 104.787)), module, TL_Drum5::LED_SN_LIGHT));

	}
};


Model* modelTL_Drum5 = createModel<TL_Drum5, TL_Drum5Widget>("TL-Drum5");