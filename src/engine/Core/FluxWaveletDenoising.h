#pragma once
#include <array>
#include <vector>
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/Matrix.h"
#include "WaveletImage.h"


class FluxWaveletDenoising {
protected:
	WaveletData* _waveletData;
public:

	static inline int GetDwtLength(int inputLength, int passSize)
	{
		return (inputLength + passSize - 1) / 2;
	}

	static inline int GetIdwtLength(int hiLength, int passSize)
	{
		return 2 * hiLength - passSize + 2;
	}

	inline int GetMaxDepth(int x, int y) {
		//TODO: Handle cases when input length is less than passSize (wavelet coeffs length)
		//For now i divide data length to overcome this limitation
		float dataLength = (float)std::min(x,y);
		dataLength /= _waveletData->Size;
		dataLength = std::floor(dataLength);
		return std::log2f(dataLength / ((float)_waveletData->Size - 1));
	}

	inline void Dispose() {
		if (_waveletData != nullptr) {
			_waveletData->Dispose();
			_waveletData = nullptr;
		}
	}

	FluxWaveletDenoising(WaveletData* data);

	WaveletLine Dwt(pixel_t* input, int length);

	WaveletImage<pixel_t> Dwt2d(Matrix<pixel_t> &input);

	Matrix<pixel_t> Idwt2d(WaveletImage<pixel_t>& inputImage);

	pixel_t* Idwt(WaveletLine& line);	

	std::vector<WaveletImage<pixel_t>> Wavedec(Matrix<pixel_t> &input);

	~FluxWaveletDenoising();
};
