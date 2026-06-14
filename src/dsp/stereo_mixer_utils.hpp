#pragma once

#include "dsp_utils.hpp"

namespace TeknoDSP {

struct PanCache {
	float lastPanKnob = 0.f;
	float monoLeft = 0.7071f;
	float monoRight = 0.7071f;
	float balanceAttenuation = 1.f;
	bool valid = false;

	void reset() {
		lastPanKnob = 0.f;
		monoLeft = 0.7071f;
		monoRight = 0.7071f;
		balanceAttenuation = 1.f;
		valid = false;
	}

	void update(float panKnob, bool panCvConnected) {
		if (panCvConnected)
			return;

		if (!valid || DSPUtils::changedEnough(panKnob, lastPanKnob)) {
			lastPanKnob = panKnob;
			DSPUtils::equalPowerGains(panKnob, monoLeft, monoRight);
			balanceAttenuation = DSPUtils::equalPowerAttenuation(panKnob);
			valid = true;
		}
	}
};

struct VuMeter5 {
	float value = 0.f;

	void reset() {
		value = 0.f;
	}

	void process(float sampleAbs, float release = 0.02f) {
		value = std::max(sampleAbs, value * (1.f - release) + sampleAbs * release);
	}

	template <typename Lights>
	void write(Lights& lights, int baseLight, float fullScale = 5.f) const {
		lights[baseLight + 0].setBrightness(value >= 0.05f * fullScale);
		lights[baseLight + 1].setBrightness(value >= 0.12f * fullScale);
		lights[baseLight + 2].setBrightness(value >= 0.25f * fullScale);
		lights[baseLight + 3].setBrightness(value >= 0.50f * fullScale);
		lights[baseLight + 4].setBrightness(value >= 0.90f * fullScale);
	}
};

}
