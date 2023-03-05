#pragma once
#include "FLXDefines.h"
#include <array>



class PixelsHelper {
public:
	template<typename T>
	static inline void Deinterleave(T* rgb, T* rOut, T* gOut, T* bOut, int width, int height)
	{
		int size = width * height * 3;
		//TODO: simd optimisation
#pragma omp parallel for
		for (int i = 0; i < size; i++)
		{
			if (i % 3 == 0)
				rOut[i / 3] = rgb[i];
			else if (i % 3 == 1)
				gOut[(i - 1) / 3] = rgb[i];
			else
				bOut[(i - 2) / 3] = rgb[i];
		}
	}

	
};


