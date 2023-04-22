#pragma once
#include <array>
#include <vector>
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/InteropTypes.h"
#include "WaveletImage.h"
#include "../infrastructure/Matrix.h"


class FluxWaveletDenoising {
protected:
	WaveletData* _waveletData;
public:

	//Iterators


	

	static inline int GetDwtLength(int inputLength, int passSize)
	{
		return (inputLength + passSize - 1) / 2;
	}

	static inline int GetIdwtLength(int hiLength, int passSize)
	{
		return 2 * hiLength - passSize + 2;
	}

	inline void Dispose() {
		if (_waveletData != nullptr) {
			_waveletData->Dispose();
			_waveletData = nullptr;
		}
	}

	FluxWaveletDenoising(WaveletData* data);

	WaveletLine Dwt(pixel_t* input, int length);

	void DwtHorizontal(Matrix<pixel_t> input, Matrix<pixel_t> &hi, Matrix<pixel_t> &lo);

	WaveletImage Dwt2d(Matrix<pixel_t> &input);

	void Idwt(WaveletLine& line, pixel_t* buffer, int startIdx = 0);	

	~FluxWaveletDenoising();

};
