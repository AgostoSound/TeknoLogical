#include "plugin.hpp"
#include "../helpers/dsp_utils.hpp"

#include <cmath>


struct TL_Static : Module {
	enum ParamId {
		FILTER_PARAM,
		NOISE_SELECTOR_PARAM,
		VOLUME_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		IN_FILTER_INPUT,
		IN_VOL_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		BLUE_OUTPUT,
		BROWN_OUTPUT,
		MIX_OUTPUT,
		PINK_OUTPUT,
		WHITE_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	DSPUtils::LowPassFilter whiteFilter;
	DSPUtils::LowPassFilter pinkFilter;
	DSPUtils::LowPassFilter brownFilter;
	DSPUtils::LowPassFilter blueFilter;
	DSPUtils::HP1 brownDcBlocker;
	DSPUtils::LowPassFilter blueShaper;

	float sampleRate = 44100.f;
	float lastCutoffHz = -1.f;

	float pinkB0 = 0.f;
	float pinkB1 = 0.f;
	float pinkB2 = 0.f;
	float pinkB3 = 0.f;
	float pinkB4 = 0.f;
	float pinkB5 = 0.f;
	float pinkB6 = 0.f;

	float brownState = 0.f;
	float blueLastLow = 0.f;

	TL_Static() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(FILTER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NOISE_SELECTOR_PARAM, 0.f, 1.f, 0.f, "");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.f, "");

		configInput(IN_FILTER_INPUT, "");
		configInput(IN_VOL_INPUT, "");

		configOutput(BLUE_OUTPUT, "");
		configOutput(BROWN_OUTPUT, "");
		configOutput(MIX_OUTPUT, "");
		configOutput(PINK_OUTPUT, "");
		configOutput(WHITE_OUTPUT, "");

		updateFilterCutoff(20000.f);
		brownDcBlocker.setCutoff(20.f, sampleRate);
		blueShaper.setCutoff(1800.f, sampleRate);
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		sampleRate = e.sampleRate;
		lastCutoffHz = -1.f;
		brownDcBlocker.setCutoff(20.f, sampleRate);
		blueShaper.setCutoff(1800.f, sampleRate);
		updateFilterCutoff(resolveCutoffHz());
	}

	void onReset() override {
		lastCutoffHz = -1.f;

		pinkB0 = 0.f;
		pinkB1 = 0.f;
		pinkB2 = 0.f;
		pinkB3 = 0.f;
		pinkB4 = 0.f;
		pinkB5 = 0.f;
		pinkB6 = 0.f;

		brownState = 0.f;
		blueLastLow = 0.f;

		brownDcBlocker.reset();
		whiteFilter = {};
		pinkFilter = {};
		brownFilter = {};
		blueFilter = {};
		blueShaper = {};

		brownDcBlocker.setCutoff(20.f, sampleRate);
		blueShaper.setCutoff(1800.f, sampleRate);
		updateFilterCutoff(resolveCutoffHz());
	}

	float randomBipolar() const {
		return 2.f * random::uniform() - 1.f;
	}

	float resolveCutoffHz() {
		float control = inputs[IN_FILTER_INPUT].isConnected()
			? clamp(inputs[IN_FILTER_INPUT].getVoltage(), 0.f, 10.f) / 10.f
			: clamp(params[FILTER_PARAM].getValue(), 0.f, 1.f);

		return std::pow(10.f, rescale(control, 0.f, 1.f, std::log10(20.f), std::log10(20000.f)));
	}

	void updateFilterCutoff(float cutoffHz) {
		if (!DSPUtils::changedEnough(cutoffHz, lastCutoffHz) || sampleRate <= 0.f)
			return;

		const float limitedCutoff = std::min(cutoffHz, 0.45f * sampleRate);
		whiteFilter.setCutoff(limitedCutoff, sampleRate);
		pinkFilter.setCutoff(limitedCutoff, sampleRate);
		brownFilter.setCutoff(limitedCutoff, sampleRate);
		blueFilter.setCutoff(limitedCutoff, sampleRate);
		lastCutoffHz = cutoffHz;
	}

	float generatePinkNoise() {
		float white = randomBipolar();

		pinkB0 = 0.99886f * pinkB0 + white * 0.0555179f;
		pinkB1 = 0.99332f * pinkB1 + white * 0.0750759f;
		pinkB2 = 0.96900f * pinkB2 + white * 0.1538520f;
		pinkB3 = 0.86650f * pinkB3 + white * 0.3104856f;
		pinkB4 = 0.55000f * pinkB4 + white * 0.5329522f;
		pinkB5 = -0.7616f * pinkB5 - white * 0.0168980f;

		float pink = pinkB0 + pinkB1 + pinkB2 + pinkB3 + pinkB4 + pinkB5 + pinkB6 + white * 0.5362f;
		pinkB6 = white * 0.115926f;

		return 0.18f * pink;
	}

	float generateBrownNoise() {
		float white = randomBipolar();
		brownState = 0.995f * brownState + white * 0.09f;
		brownState = clamp(brownState, -1.5f, 1.5f);
		return 3.5f * brownDcBlocker.process(brownState);
	}

	float generateBlueNoise() {
		float white = randomBipolar();
		float low = blueShaper.process(white);
		float blue = white - low + 0.5f * blueLastLow;
		blueLastLow = low;
		return 2.4f * blue;
	}

	void process(const ProcessArgs& args) override {
		if (args.sampleRate != sampleRate) {
			sampleRate = args.sampleRate;
			lastCutoffHz = -1.f;
			brownDcBlocker.setCutoff(20.f, sampleRate);
			blueShaper.setCutoff(1800.f, sampleRate);
		}

		updateFilterCutoff(resolveCutoffHz());

		float volume = DSPUtils::resolveVolume01(
			params[VOLUME_PARAM].getValue() * 10.f,
			inputs[IN_VOL_INPUT].isConnected(),
			inputs[IN_VOL_INPUT].getVoltage()
		);

		float white = 5.f * randomBipolar();
		float pink = 5.f * generatePinkNoise();
		float brown = 5.f * generateBrownNoise();
		float blue = 5.f * generateBlueNoise();

		white = DSPUtils::softLimit5V(whiteFilter.process(white) * volume);
		pink = DSPUtils::softLimit5V(pinkFilter.process(pink) * volume);
		brown = DSPUtils::softLimit5V(brownFilter.process(brown) * volume);
		blue = DSPUtils::softLimit5V(blueFilter.process(blue) * volume);

		outputs[WHITE_OUTPUT].setVoltage(white);
		outputs[PINK_OUTPUT].setVoltage(pink);
		outputs[BROWN_OUTPUT].setVoltage(brown);
		outputs[BLUE_OUTPUT].setVoltage(blue);
		outputs[MIX_OUTPUT].setVoltage(0.f);
	}
};


struct TL_StaticWidget : ModuleWidget {
	TL_StaticWidget(TL_Static* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Static.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.257, 34.699)), module, TL_Static::NOISE_SELECTOR_PARAM));

		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(7.911, 105.097)), module, TL_Static::FILTER_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(8.005, 66.358)), module, TL_Static::VOLUME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.057, 91.815)), module, TL_Static::IN_FILTER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.057, 79.303)), module, TL_Static::IN_VOL_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(23.275, 97.187)), module, TL_Static::BLUE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(23.219, 85.353)), module, TL_Static::BROWN_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(23.15, 109.122)), module, TL_Static::MIX_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(23.25, 73.424)), module, TL_Static::PINK_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(23.197, 61.587)), module, TL_Static::WHITE_OUTPUT));
	}
};


Model* modelTL_Static = createModel<TL_Static, TL_StaticWidget>("TL_Static");
