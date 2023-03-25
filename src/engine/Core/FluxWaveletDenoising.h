#pragma once
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/InteropTypes.h"


class FluxWaveletDenoising {
public:
	/// <summary>
	/// Performs discrete wavelet transform using Periodic padding
	/// </summary>
	/// <param name="inPixels"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	static void DWT1D(pixel_t* inPixels, int width, int height);
};
