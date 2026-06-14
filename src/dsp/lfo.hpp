#pragma once

#include "dsp_utils.hpp"

namespace TeknoDSP {

enum class LfoWave {
	Sine = 0,
	Triangle = 1,
	Square = 2
};

inline float sineWave(float phase01) {
	return std::sin(2.f * (float) M_PI * phase01);
}

inline float triangleWave(float phase01) {
	return 1.f - 4.f * std::fabs(phase01 - 0.5f);
}

inline float squareWave(float phase01) {
	return phase01 < 0.5f ? 1.f : -1.f;
}

inline float evalLfoWave(int waveType, float phase01) {
	switch ((LfoWave) waveType) {
		case LfoWave::Sine:
			return sineWave(phase01);
		case LfoWave::Triangle:
			return triangleWave(phase01);
		case LfoWave::Square:
			return squareWave(phase01);
		default:
			return sineWave(phase01);
	}
}

}
