#pragma once

#include <xsimd/xsimd.hpp>

#define pixel_t float
#define byte_t uint8_t

#ifdef __AVX2__
	#define SIMD_ARCH xsimd::avx2	
#endif // AVX2

#define vfloat xsimd::batch<pixel_t, SIMD_ARCH>
#define vint xsimd::batch<int, SIMD_ARCH>

constexpr int LUT_DIM = 32;






