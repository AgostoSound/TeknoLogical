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



// --------------------   Main cycle logic  --------------------------------------
	void process(const ProcessArgs& args) override {
		float sampleRate = args.sampleRate;

		// Get triggers.
		float kkTrig, clTrig, snTrig, chTrig, ohTrig;
		kkTrig = inputs[IN_KK_INPUT].getVoltage();
		clTrig = inputs[IN_CL_INPUT].getVoltage();
		snTrig = inputs[IN_SN_INPUT].getVoltage();
		chTrig = inputs[IN_CH_INPUT].getVoltage();
		ohTrig = inputs[IN_OH_INPUT].getVoltage();
		// Get vols.
		float kkVol, clVol, snVol, chVol, ohVol;
		kkVol = params[VOL_KK_PARAM].getValue();
		clVol = params[VOL_CL_PARAM].getValue();
		snVol = params[VOL_SN_PARAM].getValue();
		chVol = params[VOL_CH_PARAM].getValue();
		ohVol = params[VOL_OH_PARAM].getValue();
		// Get pushes.
		float kkPush, clPush, snPush, chPush, ohPush;
		kkPush = params[PUSH_KK_PARAM].getValue();
		clPush = params[PUSH_CL_PARAM].getValue();
		snPush = params[PUSH_SN_PARAM].getValue();
		chPush = params[PUSH_CH_PARAM].getValue();
		ohPush = params[PUSH_OH_PARAM].getValue();
		// Get filters.
		float kkFilter, clFilter, snFilter, chFilter, ohFilter;
		kkFilter = params[FILTER_KK_PARAM].getValue();
		clFilter = params[FILTER_CL_PARAM].getValue();
		snFilter = params[FILTER_SN_PARAM].getValue();
		chFilter = params[FILTER_CH_PARAM].getValue();
		ohFilter = params[FILTER_OH_PARAM].getValue();
		// Get decays.
		float kkDecay, clDecay, snDecay, chDecay, ohDecay;
		kkDecay = params[DECAY_KK_PARAM].getValue();
		clDecay = params[DECAY_CL_PARAM].getValue();
		snDecay = params[DECAY_SN_PARAM].getValue();
		chDecay = params[DECAY_CH_PARAM].getValue();
		ohDecay = params[DECAY_OH_PARAM].getValue();
		// Get links.
		float kkLink, clLink, snLink, chLink, ohLink;
		kkLink = params[LINK_KK_PARAM].getValue();
		clLink = params[LINK_CL_PARAM].getValue();
		snLink = params[LINK_SN_PARAM].getValue();
		chLink = params[LINK_CH_PARAM].getValue();
		ohLink = params[LINK_OH_PARAM].getValue();


		// Triggers play samples.
        if (trigKick.process(kkTrig)) 
		{kick.trigger(kick_sample, kick_sample_len); kickEnvelope.trigger(kkDecay, sampleRate);}
        if (trigClap.process(clTrig)) 
		{clap.trigger(clap_sample, clap_sample_len); clapEnvelope.trigger(clDecay, sampleRate);}
        if (trigSnare.process(snTrig)) 
		{snare.trigger(snare_sample, snare_sample_len); snareEnvelope.trigger(snDecay, sampleRate);}
        if (trigClosedHat.process(chTrig)) 
		{closedHat.trigger(closedhat_sample, closedhat_sample_len); closedHatEnvelope.trigger(chDecay, sampleRate);}
        if (trigOpenHat.process(ohTrig)) 
		{openHat.trigger(openhat_sample, openhat_sample_len); openHatEnvelope.trigger(ohDecay, sampleRate);}

		// Variables para mezcla estéreo
		float mixLeft = 0.f;
		float mixRight = 0.f;

		// Kick
		float kickSample = kick.step();
		kickSample *= kickEnvelope.process(); 
		kickSample = DSPUtils::applyBoost(kickSample, kkPush);
		kickSample = DSPUtils::applyLowPassFilter(kickSample, kkFilter, sampleRate, kickLowFilter);
		kickSample = DSPUtils::applyHighPassFilter(kickSample, kkFilter, sampleRate, kickHighFilter);
		kickSample = DSPUtils::applyVolume(kickSample, kkVol);		
		outputs[OUT_KK_OUTPUT].setVoltage(kickSample * 5.f);
		lights[LED_KK_LIGHT].setBrightness(std::fabs(kickSample));
		if (kkLink == 0.f) 
		{
			float kickLeft, kickRight;
			DSPUtils::applyPan(kickSample, params[PAN_KK_PARAM].getValue(), kickLeft, kickRight);
			mixLeft += kickLeft;
			mixRight += kickRight;
		}
		
		
		
		// Clap
		float clapSample = clap.step();
		clapSample *= clapEnvelope.process(); 
		clapSample = DSPUtils::applyBoost(clapSample, clPush);
		clapSample = DSPUtils::applyLowPassFilter(clapSample, clFilter, sampleRate, clapLowFilter);
		clapSample = DSPUtils::applyHighPassFilter(clapSample, clFilter, sampleRate, clapHighFilter);
		clapSample = DSPUtils::applyVolume(clapSample, clVol);
		outputs[OUT_CL_OUTPUT].setVoltage(clapSample * 5.f);
		lights[LED_CL_LIGHT].setBrightness(std::fabs(clapSample));
		if (clLink == 0.f) 
		{
			float clapLeft, clapRight;
			DSPUtils::applyPan(clapSample, params[PAN_CL_PARAM].getValue(), clapLeft, clapRight);
			mixLeft += clapLeft;
			mixRight += clapRight;
		}
		
		
		
		// Snare
		float snareSample = snare.step();
		snareSample *= snareEnvelope.process();
		snareSample = DSPUtils::applyBoost(snareSample, snPush);
		snareSample = DSPUtils::applyLowPassFilter(snareSample, snFilter, sampleRate, snareLowFilter);
		snareSample = DSPUtils::applyHighPassFilter(snareSample, snFilter, sampleRate, snareHighFilter);
		snareSample = DSPUtils::applyVolume(snareSample, snVol);
		outputs[OUT_SN_OUTPUT].setVoltage(snareSample * 5.f);
		lights[LED_SN_LIGHT].setBrightness(std::fabs(snareSample));
		if (snLink == 0.f) 
		{
			float snareLeft, snareRight;
			DSPUtils::applyPan(snareSample, params[PAN_SN_PARAM].getValue(), snareLeft, snareRight);
			mixLeft += snareLeft;
			mixRight += snareRight;
		}
		
		
		// Closed Hat
		float closedHatSample = closedHat.step();
		closedHatSample *= closedHatEnvelope.process();
		closedHatSample = DSPUtils::applyBoost(closedHatSample, chPush);
		closedHatSample = DSPUtils::applyLowPassFilter(closedHatSample, chFilter, sampleRate, closedHatLowFilter);
		closedHatSample = DSPUtils::applyHighPassFilter(closedHatSample, chFilter, sampleRate, closedHatHighFilter);
		closedHatSample = DSPUtils::applyVolume(closedHatSample, chVol);
		outputs[OUT_CH_OUTPUT].setVoltage(closedHatSample * 5.f);
		lights[LED_CH_LIGHT].setBrightness(std::fabs(closedHatSample));
		if (chLink == 0.f) 
		{
			float closedHatLeft, closedHatRight;
			DSPUtils::applyPan(closedHatSample, params[PAN_CH_PARAM].getValue(), closedHatLeft, closedHatRight);
			mixLeft += closedHatLeft;
			mixRight += closedHatRight;
		}
		
		
		// Open Hat
		float openHatSample = openHat.step();
		openHatSample *= openHatEnvelope.process();
		openHatSample = DSPUtils::applyBoost(openHatSample, ohPush);
		openHatSample = DSPUtils::applyLowPassFilter(openHatSample, ohFilter, sampleRate, openHatLowFilter);
		openHatSample = DSPUtils::applyHighPassFilter(openHatSample, ohFilter, sampleRate, openHatHighFilter);
		openHatSample = DSPUtils::applyVolume(openHatSample, ohVol);
		outputs[OUT_OH_OUTPUT].setVoltage(openHatSample * 5.f);
		lights[LED_OH_LIGHT].setBrightness(std::fabs(openHatSample));
		if (ohLink == 0.f) 
		{
			float openHatLeft, openHatRight;
			DSPUtils::applyPan(openHatSample, params[PAN_OH_PARAM].getValue(), openHatLeft, openHatRight);
			mixLeft += openHatLeft;
			mixRight += openHatRight;
		}


        // Salidas estéreo, escalado a ±5V
		outputs[OUT_L_OUTPUT].setVoltage(mixLeft * 5.f);
		outputs[OUT_R_OUTPUT].setVoltage(mixRight * 5.f);

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