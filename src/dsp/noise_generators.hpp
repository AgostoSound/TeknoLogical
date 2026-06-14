#pragma once

#include "dsp_utils.hpp"

namespace NoiseGenerators {

inline float randomBipolar() {
	return 2.f * random::uniform() - 1.f;
}

struct PinkNoiseGenerator {
	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float b3 = 0.f;
	float b4 = 0.f;
	float b5 = 0.f;
	float b6 = 0.f;

	void reset() {
		b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.f;
	}

	float process() {
		float white = randomBipolar();

		b0 = 0.99886f * b0 + white * 0.0555179f;
		b1 = 0.99332f * b1 + white * 0.0750759f;
		b2 = 0.96900f * b2 + white * 0.1538520f;
		b3 = 0.86650f * b3 + white * 0.3104856f;
		b4 = 0.55000f * b4 + white * 0.5329522f;
		b5 = -0.7616f * b5 - white * 0.0168980f;

		float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
		b6 = white * 0.115926f;

		return 0.18f * pink;
	}
};

struct BrownNoiseGenerator {
	DSPUtils::HP1 dcBlocker;
	float state = 0.f;

	void configure(float sampleRate) {
		dcBlocker.setCutoff(20.f, sampleRate);
	}

	void reset(float sampleRate) {
		state = 0.f;
		dcBlocker.reset();
		configure(sampleRate);
	}

	float process() {
		float white = randomBipolar();
		state = 0.995f * state + white * 0.09f;
		state = clamp(state, -1.5f, 1.5f);
		return 3.5f * dcBlocker.process(state);
	}
};

struct BlueNoiseGenerator {
	DSPUtils::LowPassFilter shaper;
	float lastLow = 0.f;

	void configure(float sampleRate) {
		shaper.setCutoff(1800.f, sampleRate);
	}

	void reset(float sampleRate) {
		lastLow = 0.f;
		shaper = {};
		configure(sampleRate);
	}

	float process() {
		float white = randomBipolar();
		float low = shaper.process(white);
		float blue = white - low + 0.5f * lastLow;
		lastLow = low;
		return 2.4f * blue;
	}
};

}
