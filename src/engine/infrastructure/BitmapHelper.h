#pragma once
#include <cmath>
#include <iostream>
#include "FLXDefines.h"


class BitmapHelper {
public:
	static int GetStride(int width, int height, int bitsPerPixel = 24);

	static int GetStrideDifference(int width, int height, int bitsPerPixel = 24);

	//TODO: Simd optimizations
	template <typename T>
	static inline void BitmapToRgb(T* rgb, T* outBuf, int width, int height, int channelsCount = 1) {
		int stride = GetStride(width, height, sizeof(T) * 8 * channelsCount);
		int paddedItems = stride / (sizeof(T) * width);
		int strideMargin = GetStrideDifference(width, height, sizeof(T) * 8 * channelsCount);
		int littleStride = GetStride(width, height, sizeof(T) * 8);

#pragma omp parallel for
		for (int y = 0; y < height; y++)
		{
			int row = (y * stride);

			for (int x = 0; x < width; x++)
			{
				int rIndex = 3 * (y * width + x) + 0;
				int gIndex = 3 * (y * width + x) + 1;
				int bIndex = 3 * (y * width + x) + 2;

				int bBitIndex = y * stride + x * 3 + 0;
				int gBitIndex = y * stride + x * 3 + 1;
				int rBitIndex = y * stride + x * 3 + 2;

				outBuf[rIndex] = rgb[rBitIndex];
				outBuf[gIndex] = rgb[gBitIndex];
				outBuf[bIndex] = rgb[bBitIndex];
			}
		}
	}
};
