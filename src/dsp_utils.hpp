#pragma once
#include "plugin.hpp"


// General DSP.
namespace DSPUtils {

	inline float applyVolume(float signal, float volumeParam) {
		float gain = clamp(volumeParam / 10.f, 0.f, 1.f);  // volumen normalizado [0..1]
		return signal * gain;
	}

	inline void applyPan(float input, float panParam, float& left, float& right) {
		// Pan simple con pan en [-1, 1]
		float pan = clamp(panParam, -1.f, 1.f);
		left  = input * (pan <= 0.f ? 1.f : 1.f - pan);
		right = input * (pan >= 0.f ? 1.f : 1.f + pan);
	}

	inline float applyBoost(float signal, float push) {
		if (push == 1.0f) {
			float boosted = signal * 1.5f;
			return clamp(boosted, -1.f, 1.f);  // o -5.f, 5.f según tu rango
		}
		return signal;
	}

}
