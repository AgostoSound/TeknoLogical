#pragma once

#include "dsp_utils.hpp"

namespace TeknoDSP {

struct ADSREnvelope {
	enum Stage {
		Idle,
		Attack,
		Decay,
		Sustain,
		Release
	};

	Stage stage = Idle;
	float attackTime = 0.001f;
	float decayTime = 0.001f;
	float sustainLevel = 0.f;
	float releaseTime = 0.001f;
	float env = 0.f;
	float releaseStart = 0.f;
	bool lastGate = false;

	static float knobToTime(float value, float minTime = 0.001f, float maxTime = 10.f) {
		return minTime * std::pow(maxTime / minTime, value);
	}

	void setControls(float attack, float decay, float sustain, float release) {
		attackTime = knobToTime(attack);
		decayTime = knobToTime(decay);
		sustainLevel = clamp(sustain, 0.f, 1.f);
		releaseTime = knobToTime(release);
	}

	void startRelease() {
		if (stage != Release && stage != Idle) {
			releaseStart = env;
			stage = Release;
		}
	}

	float process(bool gate, float sampleTime) {
		if (gate && !lastGate)
			stage = Attack;
		if (!gate && lastGate)
			startRelease();
		lastGate = gate;

		switch (stage) {
			case Idle:
				env = 0.f;
				break;
			case Attack:
				env += sampleTime / attackTime;
				if (env >= 1.f) {
					env = 1.f;
					stage = Decay;
				}
				break;
			case Decay:
				env -= sampleTime * (1.f - sustainLevel) / decayTime;
				if (env <= sustainLevel) {
					env = sustainLevel;
					stage = Sustain;
				}
				break;
			case Sustain:
				env = sustainLevel;
				break;
			case Release:
				env -= sampleTime * releaseStart / releaseTime;
				if (env <= 0.f) {
					env = 0.f;
					stage = Idle;
				}
				break;
		}

		env = clamp(env, 0.f, 1.f);
		return env;
	}
};

}
