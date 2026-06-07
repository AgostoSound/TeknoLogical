#include "plugin.hpp"
#include "../helpers/dsp_utils.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>


struct TL_Treex : Module {
	enum ParamId {
		CUT_3_PARAM,
		CUT_4_PARAM,
		CUT_5_PARAM,
		PAN_3_PARAM,
		PAN_4_PARAM,
		PAN_5_PARAM,
		VOL_3_PARAM,
		VOL_4_PARAM,
		VOL_5_PARAM,
		MUTE_3_PARAM,
		MUTE_4_PARAM,
		MUTE_5_PARAM,
		SOLO_3_PARAM,
		SOLO_4_PARAM,
		SOLO_5_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		L_IN_3_INPUT,
		L_IN_4_INPUT,
		L_IN_5_INPUT,
		R_IN_3_INPUT,
		R_IN_4_INPUT,
		R_IN_5_INPUT,
		VOL_IN_3_INPUT,
		VOL_IN_4_INPUT,
		VOL_IN_5_INPUT,
		PAN_IN_3_INPUT,
		PAN_IN_4_INPUT,
		PAN_IN_5_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LED_3_LIGHT,
		LED_4_LIGHT,
		LED_5_LIGHT,
		L_VU_1_LIGHT,
		L_VU_2_LIGHT,
		L_VU_3_LIGHT,
		L_VU_4_LIGHT,
		L_VU_5_LIGHT,
		R_VU_1_LIGHT,
		R_VU_2_LIGHT,
		R_VU_3_LIGHT,
		R_VU_4_LIGHT,
		R_VU_5_LIGHT,
		MUTE_3_LED,
		MUTE_4_LED,
		MUTE_5_LED,
		SOLO_3_LED,
		SOLO_4_LED,
		SOLO_5_LED,
		LIGHTS_LEN
	};

	static constexpr int CH = 3;

	DSPUtils::HP1 hpL[CH];
	DSPUtils::HP1 hpR[CH];

	float balG[CH] {};
	float lastPanKnob[CH] {};
	float panGL[CH] {};
	float panGR[CH] {};
	bool panCacheValid[CH] {};

	float cutHz = 180.f;
	float sampleRate = 44100.f;
	float vuL = 0.f;
	float vuR = 0.f;

	TL_Treex() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		static const std::vector<std::string> onoffLabels = {"Off", "On"};

		for (int c = 0; c < CH; ++c) {
			configSwitch(CUT_3_PARAM + c, 0.f, 1.f, 0.f, "Cutoff", onoffLabels);
			configParam(PAN_3_PARAM + c, -1.f, 1.f, 0.f, "Pan");
			configParam(VOL_3_PARAM + c, 0.f, 10.f, 0.f, "Vol");
			configSwitch(MUTE_3_PARAM + c, 0.f, 1.f, 0.f, "Mute", onoffLabels);
			configSwitch(SOLO_3_PARAM + c, 0.f, 1.f, 0.f, "Solo", onoffLabels);

			configInput(L_IN_3_INPUT + c, "L audio");
			configInput(R_IN_3_INPUT + c, "R audio");
			configInput(VOL_IN_3_INPUT + c, "Vol CV");
			configInput(PAN_IN_3_INPUT + c, "Pan CV");

			hpL[c].setCutoff(cutHz, sampleRate);
			hpR[c].setCutoff(cutHz, sampleRate);
		}

		configOutput(OUT_L_OUTPUT, "Left");
		configOutput(OUT_R_OUTPUT, "Right");

		onReset();
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		sampleRate = e.sampleRate;

		for (int c = 0; c < CH; ++c) {
			hpL[c].setCutoff(cutHz, sampleRate);
			hpR[c].setCutoff(cutHz, sampleRate);
		}
	}

	void onReset() override {
		vuL = 0.f;
		vuR = 0.f;

		for (int c = 0; c < CH; ++c) {
			hpL[c].reset();
			hpR[c].reset();

			balG[c] = 1.f;
			lastPanKnob[c] = 0.f;
			panGL[c] = 0.7071f;
			panGR[c] = 0.7071f;
			panCacheValid[c] = false;
		}
	}

	inline void updatePanCache(int c, float panKnob, bool panCvConnected) {
		if (panCvConnected)
			return;

		if (!panCacheValid[c] || DSPUtils::changedEnough(panKnob, lastPanKnob[c])) {
			lastPanKnob[c] = panKnob;

			DSPUtils::equalPowerGains(panKnob, panGL[c], panGR[c]);
			balG[c] = DSPUtils::equalPowerAttenuation(panKnob);

			panCacheValid[c] = true;
		}
	}

	void process(const ProcessArgs& args) override {
		if (args.sampleRate != sampleRate) {
			sampleRate = args.sampleRate;

			for (int c = 0; c < CH; ++c) {
				hpL[c].setCutoff(cutHz, sampleRate);
				hpR[c].setCutoff(cutHz, sampleRate);
			}
		}

		float mixL = 0.f;
		float mixR = 0.f;

		bool anySolo = false;
		for (int c = 0; c < CH; ++c)
			anySolo |= params[SOLO_3_PARAM + c].getValue() > 0.5f;

		for (int c = 0; c < CH; ++c) {
			bool cut = params[CUT_3_PARAM + c].getValue() > 0.5f;
			bool mute = params[MUTE_3_PARAM + c].getValue() > 0.5f;
			bool solo = params[SOLO_3_PARAM + c].getValue() > 0.5f;

			lights[LED_3_LIGHT + c].setBrightness(cut ? 1.f : 0.f);
			lights[MUTE_3_LED + c].setBrightness(mute ? 1.f : 0.f);
			lights[SOLO_3_LED + c].setBrightness(solo ? 1.f : 0.f);

			if (mute || (anySolo && !solo))
				continue;

			bool lConnected = inputs[L_IN_3_INPUT + c].isConnected();
			bool rConnected = inputs[R_IN_3_INPUT + c].isConnected();

			if (!lConnected && !rConnected)
				continue;

			float inL = lConnected ? inputs[L_IN_3_INPUT + c].getVoltage() : 0.f;
			float inR = rConnected ? inputs[R_IN_3_INPUT + c].getVoltage() : 0.f;

			bool stereo = lConnected && rConnected;

			if (!stereo) {
				float mono = lConnected ? inL : inR;
				inL = mono;
				inR = mono;
			}

			if (cut) {
				inL = hpL[c].process(inL);
				inR = hpR[c].process(inR);
			}

			float vol = DSPUtils::resolveVolume01(
				params[VOL_3_PARAM + c].getValue(),
				inputs[VOL_IN_3_INPUT + c].isConnected(),
				inputs[VOL_IN_3_INPUT + c].getVoltage()
			);

			inL *= vol;
			inR *= vol;

			bool panCvConnected = inputs[PAN_IN_3_INPUT + c].isConnected();

			float pan = panCvConnected
				? DSPUtils::resolvePanMinus1to1(0.f, true, inputs[PAN_IN_3_INPUT + c].getVoltage())
				: params[PAN_3_PARAM + c].getValue();

			updatePanCache(c, pan, panCvConnected);

			if (stereo) {
				if (panCvConnected) {
					float g = DSPUtils::equalPowerAttenuation(pan);

					if (pan > 0.f)
						inL *= g;
					else if (pan < 0.f)
						inR *= g;
				}
				else {
					if (pan > 0.f)
						inL *= balG[c];
					else if (pan < 0.f)
						inR *= balG[c];
				}

				mixL += inL;
				mixR += inR;
			}
			else {
				float mono = 0.5f * (inL + inR);

				if (panCvConnected) {
					float gl = 0.f;
					float gr = 0.f;
					DSPUtils::equalPowerGains(pan, gl, gr);

					mixL += mono * gl;
					mixR += mono * gr;
				}
				else {
					mixL += mono * panGL[c];
					mixR += mono * panGR[c];
				}
			}
		}

		float outL = DSPUtils::softLimit5V(mixL);
		float outR = DSPUtils::softLimit5V(mixR);

		const float rel = 0.02f;

		float absL = std::fabs(outL);
		float absR = std::fabs(outR);

		vuL = std::max(absL, vuL * (1.f - rel) + absL * rel);
		vuR = std::max(absR, vuR * (1.f - rel) + absR * rel);

		auto setVU = [&](float value, int baseLight) {
			const float fs = 5.f;

			lights[baseLight + 0].setBrightness(value >= 0.05f * fs);
			lights[baseLight + 1].setBrightness(value >= 0.12f * fs);
			lights[baseLight + 2].setBrightness(value >= 0.25f * fs);
			lights[baseLight + 3].setBrightness(value >= 0.50f * fs);
			lights[baseLight + 4].setBrightness(value >= 0.90f * fs);
		};

		setVU(vuL, L_VU_1_LIGHT);
		setVU(vuR, R_VU_1_LIGHT);

		outputs[OUT_L_OUTPUT].setVoltage(outL);
		outputs[OUT_R_OUTPUT].setVoltage(outR);
	}
};


struct TL_TreexWidget : ModuleWidget {
	TL_TreexWidget(TL_Treex* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Treex.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float X_SHIFT = 31.05f;
		const float Y_SHIFT = 11.113f;

		auto P = [&](float x, float y) {
			return mm2px(Vec(x - X_SHIFT, y + Y_SHIFT));
		};

		addParam(createParamCentered<NKK>(P(41.3, 48.233), module, TL_Treex::CUT_3_PARAM));
		addParam(createParamCentered<NKK>(P(60.922, 46.174), module, TL_Treex::CUT_4_PARAM));
		addParam(createParamCentered<NKK>(P(81.346, 48.171), module, TL_Treex::CUT_5_PARAM));

		addParam(createParamCentered<Trimpot>(P(41.4, 62.026), module, TL_Treex::PAN_3_PARAM));
		addParam(createParamCentered<Trimpot>(P(61.05, 59.835), module, TL_Treex::PAN_4_PARAM));
		addParam(createParamCentered<Trimpot>(P(81.4, 61.898), module, TL_Treex::PAN_5_PARAM));

		addParam(createParamCentered<Rogan1PWhite>(P(41.238, 75.329), module, TL_Treex::VOL_3_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(P(61.025, 73.204), module, TL_Treex::VOL_4_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(P(81.4, 75.267), module, TL_Treex::VOL_5_PARAM));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(36.764, 88.753), 
		module, TL_Treex::MUTE_3_PARAM, TL_Treex::MUTE_3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(57.31, 86.156), 
		module, TL_Treex::MUTE_4_PARAM, TL_Treex::MUTE_4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(77.6, 87.792), 
		module, TL_Treex::MUTE_5_PARAM, TL_Treex::MUTE_5_LED));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(45.29, 87.753), 
		module, TL_Treex::SOLO_3_PARAM, TL_Treex::SOLO_3_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(65.6, 86.141), 
		module, TL_Treex::SOLO_4_PARAM, TL_Treex::SOLO_4_LED));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<BlueLight>>>(P(86.05, 88.777), 
		module, TL_Treex::SOLO_5_PARAM, TL_Treex::SOLO_5_LED));

		addInput(createInputCentered<PJ301MPort>(P(41.169, 6.017), module, TL_Treex::L_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(60.956, 3.892), module, TL_Treex::L_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(81.215, 5.955), module, TL_Treex::L_IN_5_INPUT));

		addInput(createInputCentered<PJ301MPort>(P(41.169, 15.564), module, TL_Treex::R_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(60.956, 13.439), module, TL_Treex::R_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(81.215, 15.502), module, TL_Treex::R_IN_5_INPUT));

		addInput(createInputCentered<PJ301MPort>(P(41.18, 27.162), module, TL_Treex::VOL_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(60.967, 25.036), module, TL_Treex::VOL_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(81.226, 27.1), module, TL_Treex::VOL_IN_5_INPUT));

		addInput(createInputCentered<PJ301MPort>(P(41.158, 36.687), module, TL_Treex::PAN_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(60.945, 34.561), module, TL_Treex::PAN_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(P(81.204, 36.625), module, TL_Treex::PAN_IN_5_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(P(56.5, 105.97), module, TL_Treex::OUT_L_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(P(66.01, 106.1), module, TL_Treex::OUT_R_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(P(36.083, 45.158), module, TL_Treex::LED_3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(55.87, 43.033), module, TL_Treex::LED_4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(76.129, 45.096), module, TL_Treex::LED_5_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(P(9.6, 115.7), module, TL_Treex::L_VU_1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(15.1, 115.7), module, TL_Treex::L_VU_2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(20.5, 115.7), module, TL_Treex::L_VU_3_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(P(26.3, 115.7), module, TL_Treex::L_VU_4_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(P(33.0, 115.7), module, TL_Treex::L_VU_5_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(P(112.7, 115.7), module, TL_Treex::R_VU_1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(107.2, 115.7), module, TL_Treex::R_VU_2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(P(101.8, 115.7), module, TL_Treex::R_VU_3_LIGHT));
		addChild(createLightCentered<SmallLight<WhiteLight>>(P(96.0, 115.7), module, TL_Treex::R_VU_4_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(P(89.3, 115.7), module, TL_Treex::R_VU_5_LIGHT));
	}
};

Model* modelTL_Treex = createModel<TL_Treex, TL_TreexWidget>("TL_Treex");