#pragma once

#include "dsp_utils.hpp"

namespace TeknoDSP {

struct PumpEnvelope {
	struct ShapeDef {
		float hold;
		float release;
		float releasePow;
		float fade;
		float fadePow;
		bool latchZero;
	};

	static constexpr float minGain = 0.03f;

	ShapeDef shapes[4] = {
		{0.040f, 0.280f, 0.70f, 0.000f, 1.00f, false},
		{0.040f, 0.200f, 2.40f, 0.000f, 1.00f, false},
		{0.000f, 0.000f, 1.00f, 0.350f, 1.10f, true},
		{0.020f, 0.140f, 0.75f, 0.000f, 1.00f, false},
	};

	bool active = false;
	float time = 0.f;

	void trigger() {
		active = true;
		time = 0.f;
	}

	float gainFromShape(int idx, float t) const {
		const ShapeDef& s = shapes[idx];

		if (idx == 2) {
			float x = s.fade > 0.f ? t / s.fade : 1.f;
			x = clamp(x, 0.f, 1.f);
			return clamp(1.f - std::pow(x, s.fadePow), 0.f, 1.f);
		}

		if (t <= s.hold)
			return minGain;

		float x = s.release > 0.f ? (t - s.hold) / s.release : 1.f;
		x = clamp(x, 0.f, 1.f);
		float shaped = std::pow(x, s.releasePow);
		return clamp(minGain + (1.f - minGain) * shaped, minGain, 1.f);
	}

	float process(int shapeIndex, float sampleTime) {
		if (!active)
			return 1.f;

		shapeIndex = clamp(shapeIndex, 0, 3);
		float gain = gainFromShape(shapeIndex, time);
		time += sampleTime;

		const ShapeDef& s = shapes[shapeIndex];
		if (shapeIndex == 2) {
			if (!s.latchZero && time > s.fade + 0.01f) {
				active = false;
				time = 0.f;
			}
		}
		else if (time > s.hold + s.release + 0.01f) {
			active = false;
			time = 0.f;
			gain = 1.f;
		}

		return gain;
	}
};

}
