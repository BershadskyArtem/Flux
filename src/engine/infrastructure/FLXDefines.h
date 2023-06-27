#pragma once

#include <xsimd/xsimd.hpp>

typedef float pixel_t;
typedef unsigned char byte_t;

#ifdef __AVX2__
	#define SIMD_ARCH xsimd::avx2	
#endif // AVX2

typedef xsimd::batch<pixel_t, SIMD_ARCH> vfloat;
typedef xsimd::batch<int, SIMD_ARCH> vint;

constexpr int LUT_DIM = 64;
constexpr float PI = 3.14159265359;
constexpr int MAX_WAVELET_DEPTH = 8;






