#pragma once

#include "dsp_utils.hpp"

#include <cstdint>

namespace TeknoDSP {

struct Int16SampleVoice {
	const int16_t* sample = nullptr;
	int length = 0;
	float pos = 0.f;
	float stepSize = 1.f;
	bool playing = false;

	float step() {
		if (!playing || !sample || pos >= length - 1)
			return 0.f;

		int i0 = (int) pos;
		int i1 = std::min(i0 + 1, length - 1);
		float frac = pos - i0;

		float s0 = (float) sample[i0] / 32768.f;
		float s1 = (float) sample[i1] / 32768.f;
		float out = s0 + (s1 - s0) * frac;

		pos += stepSize;
		if (pos >= length)
			playing = false;

		return out;
	}

	void trigger(const int16_t* s, int len, float currentSampleRate, float sourceSampleRate = 48000.f) {
		sample = s;
		length = len;
		pos = 0.f;
		stepSize = sourceSampleRate / currentSampleRate;
		playing = true;
	}
};

}
