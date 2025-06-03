#pragma once
#include "rack.hpp"
#include "dsp/filter.hpp"
using namespace rack;


// General DSP.
namespace DSPUtils {

    struct LowPassFilter {
        float y = 0.f;
        float alpha = 1.f;

        void setCutoff(float cutoff, float sampleRate) {
            float rc = 1.f / (2.f * M_PI * cutoff);
            float dt = 1.f / sampleRate;
            alpha = dt / (rc + dt);
            alpha = clamp(alpha, 0.f, 1.f);
        }

        float process(float input) {
            y += alpha * (input - y);
            return y;
        }
    };

    struct HighPassFilter {
        float y = 0.f;
        float x = 0.f;
        float alpha = 1.f;

        void setCutoff(float cutoff, float sampleRate) {
            float rc = 1.f / (2.f * M_PI * cutoff);
            float dt = 1.f / sampleRate;
            alpha = rc / (rc + dt);
            alpha = clamp(alpha, 0.f, 1.f);
        }

        float process(float input) {
            float out = alpha * (y + input - x);
            x = input;
            y = out;
            return out;
        }
    };

    struct DecayEnvelope {
        float value = 0.f;
        float decayCoeff = 0.f;

        void trigger(float decayParam, float sampleRate) {
            // Mapear decayParam de [-10, +10] a [0.005, 2.0] segundos (logarítmico)
            const float minTime = 0.08f;
            const float maxTime = 0.8f;
            float normalized = (decayParam + 10.f) / 20.f;
            float decayTime = minTime * std::pow(maxTime / minTime, normalized); // escala logarítmica

            // Evitar división por cero
            decayTime = std::max(decayTime, 0.001f);
            decayCoeff = std::exp(-1.f / (decayTime * sampleRate));
            value = 1.f;
        }


        float process() {
            value *= decayCoeff;
            return value;
        }

        bool isActive() const {
            return value > 0.001f;
        }
    };


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

    inline float applyLowPassFilter(float input, float filterParam, float sampleRate, LowPassFilter& filter) {
        if (filterParam < 0.f) {
            // Mapeo logarítmico: -10..0 → 50..20000 Hz
            float cutoffFreq = std::pow(10.f, rescale(filterParam, -10.f, 0.f, std::log10(20.f), std::log10(20000.f)));
            filter.setCutoff(cutoffFreq, sampleRate);
            return filter.process(input);
        }
        return input;
    }

    inline float applyHighPassFilter(float input, float filterParam, float sampleRate, HighPassFilter& filter) {
        if (filterParam > 0.f) {
            // Mapeo logarítmico: 0..+10 → 20..20000 Hz
            float cutoffFreq = std::pow(10.f, rescale(filterParam, 0.f, 10.f, std::log10(20.f), std::log10(20000.f)));
            filter.setCutoff(cutoffFreq, sampleRate);
            return filter.process(input);
        }
        return input;
    }

}
