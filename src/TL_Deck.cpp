#include "plugin.hpp"
#include "rack.hpp"
#include "dsp/digital.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <cstdlib>
#include <osdialog.h>

using namespace rack;

struct TL_Deck : Module {
	enum ParamId {
		KNOB_VOL_PARAM,
		LATCH_LOAD_PARAM,
		LATCH_PLAY_PARAM,
		LATCH_STOP_PARAM,
		SCREEN_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		CV_PLAY_INPUT,
		CV_STOP_INPUT,
		CV_VOL_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		L_OUT_OUTPUT,
		R_OUT_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LED_LOAD_LIGHT,
		LED_PLAY_LIGHT,
		LED_STOP_LIGHT,
		LATCH_LOAD_LED,
		LATCH_PLAY_LED,
		LATCH_STOP_LED,
		LIGHTS_LEN
	};

	struct SampleData {
		std::vector<float> left;
		std::vector<float> right;
		double sampleRate = 44100.0;
	};

	std::shared_ptr<SampleData> sampleData;
	std::string samplePath;
	std::vector<float> waveformPeak;

	dsp::SchmittTrigger playButtonTrigger;
	dsp::SchmittTrigger playCvTrigger;
	dsp::SchmittTrigger stopButtonTrigger;
	dsp::SchmittTrigger stopCvTrigger;

	bool playing = false;
	double playhead = 0.0;

	TL_Deck() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(KNOB_VOL_PARAM, 0.f, 1.f, 0.8f, "Volume");
		configParam(LATCH_LOAD_PARAM, 0.f, 1.f, 0.f, "Load");
		configParam(LATCH_PLAY_PARAM, 0.f, 1.f, 0.f, "Play");
		configParam(LATCH_STOP_PARAM, 0.f, 1.f, 0.f, "Stop");
		configParam(SCREEN_PARAM, 0.f, 1.f, 0.f, "Screen");

		configInput(CV_PLAY_INPUT, "Play CV");
		configInput(CV_STOP_INPUT, "Stop CV");
		configInput(CV_VOL_INPUT, "Volume CV");

		configOutput(L_OUT_OUTPUT, "Left Out");
		configOutput(R_OUT_OUTPUT, "Right Out");
	}

	void buildWaveformCache() {
		waveformPeak.clear();

		auto data = std::atomic_load(&sampleData);
		if (!data || data->left.empty())
			return;

		const int visualSize = 2048;
		waveformPeak.resize(visualSize, 0.f);

		const size_t totalSamples = data->left.size();

		for (int i = 0; i < visualSize; i++) {
			size_t start = (size_t)((double)i / visualSize * totalSamples);
			size_t end = (size_t)((double)(i + 1) / visualSize * totalSamples);
			end = std::min(end, totalSamples);

			float peak = 0.f;

			for (size_t j = start; j < end; j++) {
				float mono = 0.5f * (std::fabs(data->left[j]) + std::fabs(data->right[j]));
				peak = std::max(peak, mono);
			}

			waveformPeak[i] = peak;
		}
	}

	bool loadWavFile(const std::string& path) {
		drwav_uint64 frameCount = 0;
		unsigned int channelCount = 0;
		unsigned int fileSampleRate = 0;
		float* interleaved = drwav_open_file_and_read_pcm_frames_f32(path.c_str(), &channelCount, &fileSampleRate, &frameCount, nullptr);
		if (!interleaved || frameCount == 0 || channelCount == 0 || fileSampleRate == 0) {
			if (interleaved)
				drwav_free(interleaved, nullptr);
			return false;
		}

		auto newData = std::make_shared<SampleData>();
		newData->left.resize(static_cast<size_t>(frameCount));
		newData->right.resize(static_cast<size_t>(frameCount));
		newData->sampleRate = static_cast<double>(fileSampleRate);

		for (drwav_uint64 i = 0; i < frameCount; ++i) {
			const drwav_uint64 base = i * channelCount;
			const float l = interleaved[base + 0];
			const float r = (channelCount > 1) ? interleaved[base + 1] : l;
			newData->left[static_cast<size_t>(i)] = l;
			newData->right[static_cast<size_t>(i)] = r;
		}

		drwav_free(interleaved, nullptr);

		std::atomic_store(&sampleData, newData);
		samplePath = path;
		playing = false;
		playhead = 0.0;

		buildWaveformCache();

		return true;
	}

	void clearSample() {
		std::atomic_store(&sampleData, std::shared_ptr<SampleData>());
		samplePath.clear();
		playing = false;
		playhead = 0.0;
	}

	void startPlayback() {
		auto data = std::atomic_load(&sampleData);
		if (!data || data->left.empty()) {
			playing = false;
			playhead = 0.0;
			return;
		}
		playing = true;
		playhead = 0.0;
	}

	void stopPlayback() {
		playing = false;
		playhead = 0.0;
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		if (!samplePath.empty())
			json_object_set_new(rootJ, "samplePath", json_string(samplePath.c_str()));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* pathJ = json_object_get(rootJ, "samplePath");
		if (pathJ && json_is_string(pathJ)) {
			std::string path = json_string_value(pathJ);
			if (!path.empty() && system::exists(path)) {
				loadWavFile(path);
			}
		}
	}

	float getInterpolatedSample(const std::vector<float>& buffer, double index) const {
		const size_t n = buffer.size();
		if (n == 0)
			return 0.f;
		if (n == 1)
			return buffer[0];

		if (index <= 0.0)
			return buffer[0];

		const double maxIndex = static_cast<double>(n - 1);
		if (index >= maxIndex)
			return buffer[n - 1];

		size_t i0 = static_cast<size_t>(index);
		size_t i1 = i0 + 1;
		double frac = index - static_cast<double>(i0);
		return static_cast<float>(buffer[i0] + (buffer[i1] - buffer[i0]) * frac);
	}

	void process(const ProcessArgs& args) override {
		const bool playTrig = playButtonTrigger.process(params[LATCH_PLAY_PARAM].getValue())
			|| playCvTrigger.process(inputs[CV_PLAY_INPUT].getVoltage());
		const bool stopTrig = stopButtonTrigger.process(params[LATCH_STOP_PARAM].getValue())
			|| stopCvTrigger.process(inputs[CV_STOP_INPUT].getVoltage());

		if (playTrig)
			startPlayback();
		if (stopTrig)
			stopPlayback();

		float outL = 0.f;
		float outR = 0.f;

		auto data = std::atomic_load(&sampleData);
		if (playing && data && !data->left.empty()) {
			const size_t frameCount = data->left.size();
			if (playhead >= static_cast<double>(frameCount)) {
				stopPlayback();
			}
			else {
				float sampleL = getInterpolatedSample(data->left, playhead);
				float sampleR = getInterpolatedSample(data->right, playhead);

				float knobVol = clamp(params[KNOB_VOL_PARAM].getValue(), 0.f, 1.f);
				float vol = knobVol;
				if (inputs[CV_VOL_INPUT].isConnected()) {
					float cv01 = clamp(inputs[CV_VOL_INPUT].getVoltage() / 10.f, 0.f, 1.f);
					vol = knobVol * cv01;
				}

				outL = clamp(sampleL * 5.f * vol, -5.f, 5.f);
				outR = clamp(sampleR * 5.f * vol, -5.f, 5.f);

				playhead += data->sampleRate / args.sampleRate;
				if (playhead >= static_cast<double>(frameCount)) {
					stopPlayback();
				}
			}
		}

		outputs[L_OUT_OUTPUT].setVoltage(outL);
		outputs[R_OUT_OUTPUT].setVoltage(outR);

		const float loadedBright = (data && !data->left.empty()) ? 1.f : 0.f;
		const float playBright = playing ? 1.f : 0.f;
		const float stopBright = playing ? 0.f : 1.f;

		lights[LED_LOAD_LIGHT].setBrightness(loadedBright);
		lights[LED_PLAY_LIGHT].setBrightness(playBright);
		lights[LED_STOP_LIGHT].setBrightness(stopBright);

		// Button lights are only press feedback, not state indicators.
		lights[LATCH_LOAD_LED].setBrightness(params[LATCH_LOAD_PARAM].getValue());
		lights[LATCH_PLAY_LED].setBrightness(params[LATCH_PLAY_PARAM].getValue());
		lights[LATCH_STOP_LED].setBrightness(params[LATCH_STOP_PARAM].getValue());
	}
};

static void openDeckWavDialog(TL_Deck* module) {
	if (!module)
		return;

	osdialog_filters* filters = osdialog_filters_parse("WAV:wav");
	char* path = osdialog_file(OSDIALOG_OPEN, NULL, NULL, filters);

	if (path) {
		module->loadWavFile(path);
		std::free(path);
	}

	osdialog_filters_free(filters);
}

struct LoadWavButton : VCVLightButton<LargeSimpleLight<WhiteLight>> {
	TL_Deck* deckModule = nullptr;

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (e.action == GLFW_PRESS) {
				openDeckWavDialog(deckModule);
			}
			e.consume(this);
			return;
		}

		VCVLightButton<LargeSimpleLight<WhiteLight>>::onButton(e);
	}
};

struct DeckWaveformDisplay : TransparentWidget {
	TL_Deck* module = nullptr;

	DeckWaveformDisplay(TL_Deck* module, Vec pos, Vec size) {
		this->module = module;
		box.pos = pos;
		box.size = size;
	}

	void draw(const DrawArgs& args) override {
		// Background
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGB(4, 12, 16));
		nvgFill(args.vg);

		// Border
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.5f, 0.5f, box.size.x - 1.f, box.size.y - 1.f);
		nvgStrokeColor(args.vg, nvgRGB(0, 180, 210));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);

		if (!module || module->waveformPeak.empty())
			return;

		const float centerY = box.size.y * 0.5f;
		const float centerX = box.size.x * 0.5f;
		const float halfWidth = box.size.x * 0.42f;

		float current = 0.f;
		float total = 1.f;

		auto data = std::atomic_load(&module->sampleData);
		if (data && !data->left.empty()) {
			current = (float)module->playhead;
			total = (float)data->left.size();
		}

		const float currentNorm = clamp(current / total, 0.f, 1.f);
		const int cacheSize = (int)module->waveformPeak.size();

		// Visible vertical window around current playback position.
		const float visibleNorm = 0.08f;

		nvgBeginPath(args.vg);

		for (int y = 0; y < (int)box.size.y; y++) {
			float relY = (centerY - y) / box.size.y;
			float sampleNorm = currentNorm + relY * visibleNorm;
			float peak = 0.f;
			if (sampleNorm >= 0.f && sampleNorm <= 1.f) {
				int index = clamp((int)(sampleNorm * (cacheSize - 1)), 0, cacheSize - 1);
				peak = module->waveformPeak[index];
			}

			float x1 = centerX - peak * halfWidth;
			float x2 = centerX + peak * halfWidth;

			nvgMoveTo(args.vg, x1, y);
			nvgLineTo(args.vg, x2, y);
		}

		nvgStrokeColor(args.vg, nvgRGB(0, 220, 255));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);

		// Current playback line
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0.f, centerY);
		nvgLineTo(args.vg, box.size.x, centerY);
		nvgStrokeColor(args.vg, nvgRGB(255, 255, 255));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);
	}
};

struct TL_DeckWidget : ModuleWidget {
	struct LoadWavItem : MenuItem {
		TL_Deck* module = nullptr;

		void onAction(const event::Action& e) override {
			openDeckWavDialog(module);
		}
	};

	TL_DeckWidget(TL_Deck* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Deck.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(22.498, 60.696)), module, TL_Deck::KNOB_VOL_PARAM));

		addChild(new DeckWaveformDisplay(module, mm2px(Vec(8.52, 21.13)), mm2px(Vec(13.47, 30.24))));

		auto* loadButton = createLightParamCentered<LoadWavButton>(
			mm2px(Vec(15.115, 83.198)),
			module,
			TL_Deck::LATCH_LOAD_PARAM,
			TL_Deck::LATCH_LOAD_LED
		);
		loadButton->deckModule = module;
		addParam(loadButton);
		
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(5.393, 86.504)),
		module, TL_Deck::LATCH_PLAY_PARAM, TL_Deck::LATCH_PLAY_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(24.969, 86.091)),
		module, TL_Deck::LATCH_STOP_PARAM, TL_Deck::LATCH_STOP_LED));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.049, 96.600)), module, TL_Deck::CV_PLAY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.599, 96.395)), module, TL_Deck::CV_STOP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.032, 60.631)), module, TL_Deck::CV_VOL_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(10.058, 109.231)), module, TL_Deck::L_OUT_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(20.653, 109.306)), module, TL_Deck::R_OUT_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(15.238, 88.970)), module, TL_Deck::LED_LOAD_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(5.416, 80.663)), module, TL_Deck::LED_PLAY_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(24.961, 80.235)), module, TL_Deck::LED_STOP_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		menu->addChild(new MenuSeparator());

		auto* item = createMenuItem<LoadWavItem>("Load WAV...");
		item->module = dynamic_cast<TL_Deck*>(module);
		menu->addChild(item);
	}
};


Model* modelTL_Deck = createModel<TL_Deck, TL_DeckWidget>("TL_Deck");
