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

		for (int y = 0; y < height; y++)
		{
			byte_t* row = (byte_t*)rgb + (y * stride);

			for (int x = 0; x < width; x++)
			{
				// Watch out for actual order (BGR)!
				int bIndex = x * sizeof(T);
				int gIndex = bIndex + 1;
				int rIndex = bIndex + 2;

				T pixelR = row[rIndex];
				T pixelG = row[gIndex];
				T pixelB = row[bIndex];

				row[rIndex] = pixelR;
				row[bIndex] = pixelB;
				row[gIndex] = pixelG;
			}
		}
	}

}

