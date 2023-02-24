#pragma once

#include "FLXDefines.h"
#include <xsimd/xsimd.hpp>

class LUT
{
private:
	pixel_t* data;
	int size;
	int sizeWithPadding;
	vfloat sizeV;
	xsimd::batch<int, SIMD_ARCH> oneV;
public:
	//LUT();
	void Init(int size, pixel_t defaultValue = 1.f);
	void Dispose();
	void SetValue(pixel_t value, int index);
	pixel_t Get(pixel_t value);
	pixel_t Get(int value);
	pixel_t Get01(pixel_t value);
	inline vfloat Get01(vfloat values) {
		values *= sizeV;
		auto index = xsimd::batch_cast<int>(values);
		auto diff = values - xsimd::batch_cast<pixel_t>(index);
		auto p1 = vfloat::gather(data, index);
		auto p2 = vfloat::gather(data, index + oneV) - p1;
		return p1 + p2 * diff;
	}
	//float Value
};

