#pragma once
#include "FLXDefines.h"
#include <xsimd/xsimd.hpp>

class FluxMath {
public:
	static inline float DualSmoothstep(float input, float t3, float t1, float t21) {
		float k = std::fmax(0, std::fmin(1, (std::fabsf(input - t3) - t1) / (t21)));
		return k * k * (3.f - 2.f * k);
	}

	template <typename T>
	static inline T LinearInterpolate(T alpha, T alphaOne, T alphaZero) {
		return alphaOne * alpha + (1 - alpha) * alphaZero;
	}

	static inline pixel_t ContrastSmoothstep(float value) {
		pixel_t k = std::max(0.f, std::min(1.f, (std::abs(value * 0.5f - 0.5f) - 0.5f) / (-0.5f)));
		pixel_t s = k * k * (3.f - 2.f * k);
		return s;
	}


};