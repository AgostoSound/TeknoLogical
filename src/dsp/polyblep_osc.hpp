#pragma once

#include "dsp_utils.hpp"

namespace TeknoDSP {

struct PolyBLEPOsc {
	float phase = 0.f;
	float subPhase = 0.f;
	float freq = 100.f;
	float sr = 44100.f;
	float lastDt = 100.f / 44100.f;

	void setSampleRate(float sampleRate) {
		sr = sampleRate;
		lastDt = freq / sr;
	}

	void setFreq(float f) {
		freq = clamp(f, 10.f, 12000.f);
		lastDt = freq / sr;
	}

	void resetPhase() {
		phase = 0.f;
		subPhase = 0.f;
	}

	static inline float polyblep(float t, float dt) {
		if (t < dt) {
			t /= dt;
			return t + t - t * t - 1.f;
		}
		if (t > 1.f - dt) {
			t = (t - 1.f) / dt;
			return t * t + t + t + 1.f;
		}
		return 0.f;
	}

	inline void advance() {
		float dt = lastDt;
		phase += dt;
		if (phase >= 1.f)
			phase -= 1.f;

		subPhase += 0.5f * dt;
		if (subPhase >= 1.f)
			subPhase -= 1.f;
	}

	inline float sine() const {
		return std::sin(2.f * (float) M_PI * phase);
	}

	inline float triangle() const {
		float t = 2.f * phase - 1.f;
		return 2.f * std::fabs(t) - 1.f;
	}

	inline float sawBLEP() const {
		float x = 2.f * phase - 1.f;
		return x - polyblep(phase, lastDt);
	}

	inline float squareBLEP(float pwm = 0.5f) const {
		pwm = clamp(pwm, 0.05f, 0.95f);
		float y = (phase < pwm) ? 1.f : -1.f;
		y += polyblep(phase, lastDt);

		float t = phase - pwm;
		if (t < 0.f)
			t += 1.f;
		y -= polyblep(t, lastDt);
		return y;
	}

	inline float subSine() const {
		return std::sin(2.f * (float) M_PI * subPhase);
	}
};

}
