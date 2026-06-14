#include "plugin.hpp"
#include "../helpers/dsp_utils.hpp"
#include "../helpers/noise_generators.hpp"
#include "../helpers/widgets/noise_selector.hpp"

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
	NoiseGenerators::PinkNoiseGenerator pinkNoise;
	NoiseGenerators::BrownNoiseGenerator brownNoise;
	NoiseGenerators::BlueNoiseGenerator blueNoise;
	dsp::ClockDivider controlDivider;

	static constexpr int CONTROL_DIVISION = 8;
	float sampleRate = 44100.f;
	float lastCutoffHz = -1.f;
	float currentVolume = 0.f;
	float currentCutoffHz = 20000.f;
	float mixX = 0.5f;
	float mixY = 0.5f;

	TL_Static() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(FILTER_PARAM, 0.f, 1.f, 1.f, "Filter");
		configParam(NOISE_SELECTOR_PARAM, 0.f, 1.f, 0.5f, "Noise selector");
		configParam(VOLUME_PARAM, 0.f, 1.f, 0.8f, "Volume", "%", 0.f, 100.f);

		configInput(IN_FILTER_INPUT, "Filter CV");
		configInput(IN_VOL_INPUT, "Volume CV");

		configOutput(BLUE_OUTPUT, "Blue noise");
		configOutput(BROWN_OUTPUT, "Brown noise");
		configOutput(MIX_OUTPUT, "Mix");
		configOutput(PINK_OUTPUT, "Pink noise");
		configOutput(WHITE_OUTPUT, "White noise");

		controlDivider.setDivision(CONTROL_DIVISION);
		updateFilterCutoff(20000.f);
		brownNoise.configure(sampleRate);
		blueNoise.configure(sampleRate);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "mixX", json_real(mixX));
		json_object_set_new(rootJ, "mixY", json_real(mixY));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* mixXJ = json_object_get(rootJ, "mixX");
		json_t* mixYJ = json_object_get(rootJ, "mixY");

		if (mixXJ)
			mixX = clamp((float) json_number_value(mixXJ), 0.f, 1.f);
		if (mixYJ)
			mixY = clamp((float) json_number_value(mixYJ), 0.f, 1.f);
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		sampleRate = e.sampleRate;
		lastCutoffHz = -1.f;
		brownNoise.configure(sampleRate);
		blueNoise.configure(sampleRate);
		controlDivider.reset();
		currentCutoffHz = resolveCutoffHz();
		updateFilterCutoff(currentCutoffHz);
	}

	void onReset() override {
		lastCutoffHz = -1.f;
		pinkNoise.reset();
		brownNoise.reset(sampleRate);
		blueNoise.reset(sampleRate);
		whiteFilter = {};
		pinkFilter = {};
		brownFilter = {};
		blueFilter = {};
		controlDivider.reset();
		currentVolume = 0.f;
		currentCutoffHz = resolveCutoffHz();
		updateFilterCutoff(currentCutoffHz);
	}

	void setMixPosition(float x, float y) {
		mixX = clamp(x, 0.f, 1.f);
		mixY = clamp(y, 0.f, 1.f);
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

	float resolveVolume() {
		return DSPUtils::resolveVolume01(
			params[VOLUME_PARAM].getValue() * 10.f,
			inputs[IN_VOL_INPUT].isConnected(),
			inputs[IN_VOL_INPUT].getVoltage()
		);
	}

	float clip5V(float x) const {
		return clamp(x, -5.f, 5.f);
	}

	void process(const ProcessArgs& args) override {
		if (args.sampleRate != sampleRate) {
			sampleRate = args.sampleRate;
			lastCutoffHz = -1.f;
			brownNoise.configure(sampleRate);
			blueNoise.configure(sampleRate);
			controlDivider.reset();
		}

		if (controlDivider.process()) {
			currentCutoffHz = resolveCutoffHz();
			currentVolume = resolveVolume();
			updateFilterCutoff(currentCutoffHz);
		}

		float white = 5.f * NoiseGenerators::randomBipolar();
		float pink = 5.f * pinkNoise.process();
		float brown = 5.f * brownNoise.process();
		float blue = 5.f * blueNoise.process();

		white = clip5V(whiteFilter.process(white) * currentVolume);
		pink = clip5V(pinkFilter.process(pink) * currentVolume);
		brown = clip5V(brownFilter.process(brown) * currentVolume);
		blue = clip5V(blueFilter.process(blue) * currentVolume);

		DSPUtils::QuadMixWeights mixWeights = DSPUtils::bilinearMixWeights(mixX, mixY);

		float mix = white * mixWeights.topLeft
			+ pink * mixWeights.topRight
			+ brown * mixWeights.bottomLeft
			+ blue * mixWeights.bottomRight;
		mix = clip5V(mix);

		outputs[WHITE_OUTPUT].setVoltage(white);
		outputs[PINK_OUTPUT].setVoltage(pink);
		outputs[BROWN_OUTPUT].setVoltage(brown);
		outputs[BLUE_OUTPUT].setVoltage(blue);
		outputs[MIX_OUTPUT].setVoltage(mix);
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

		auto* noiseSelector = new XYPadWidget(
			APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/NoiseSelectorBorder.svg")),
			APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/NoiseSelectorHandler.svg")),
			mm2px(Vec(19.f, 19.f))
		);
		noiseSelector->onChange = [module](float x, float y) {
			if (module)
				module->setMixPosition(x, y);
		};
		noiseSelector->syncSource = [module](float& x, float& y) {
			if (!module)
				return;
			x = module->mixX;
			y = module->mixY;
		};
		noiseSelector->box.pos = mm2px(Vec(15.257, 34.699)).minus(noiseSelector->box.size.mult(0.5f));
		addChild(noiseSelector);

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
