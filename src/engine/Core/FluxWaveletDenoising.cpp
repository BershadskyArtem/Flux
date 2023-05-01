#include "FluxWaveletDenoising.h"
#include <iostream>

FluxWaveletDenoising::FluxWaveletDenoising(WaveletData* data)
{
	_waveletData = data;
	_zero = 0;
	_one = 1;
	_zeroV = 0;
	_oneV = 1;
	_maxV = 999999999;
	_max = 999999999;
	_minV = -999999999;
	_min = -999999999;
}



/// <summary>
/// Performs 1D Discrete Wavelet Transform
/// </summary>
/// <param name="input">Pointer to pixels</param>
/// <param name="length">Length of pixel array</param>
/// <returns></returns>
WaveletLine FluxWaveletDenoising::Dwt(pixel_t* input, int length)
{
	int resultLength = GetDwtLength(length, _waveletData->Size);
	pixel_t* hi = new pixel_t[resultLength]{};
	pixel_t* lo = new pixel_t[resultLength]{};

	int inputIdx = 1;
	int currentHiLoIdx = 0;
	//Zero padded means that we can ignore some of the values
	for (; inputIdx < _waveletData->Size - 1; inputIdx += 2, currentHiLoIdx++)
	{
		//j <= InputIdx because we if i - j < 0 => we are trying to access out of bounds
		// elements and since we use 0 padding we can just skip them all at once.
		for (int j = 0; j <= inputIdx && j < _waveletData->Size; j++)
		{
			hi[currentHiLoIdx] += input[inputIdx - j] * _waveletData->HighPassDeconstruction[j];
			lo[currentHiLoIdx] += input[inputIdx - j] * _waveletData->LowPassDeconstruction[j];
		}
	}

	//Main Part
	//j<=inputIdx cannot happen so we can remove extra check in the inner loop
	for (; inputIdx < length; inputIdx += 2, currentHiLoIdx++)
	{
		for (int j = 0; j < _waveletData->Size; j++)
		{
			int index = inputIdx - j;
			hi[currentHiLoIdx] += input[index] * _waveletData->HighPassDeconstruction[j];
			lo[currentHiLoIdx] += input[index] * _waveletData->LowPassDeconstruction[j];
		}
	}

	//Right overhang
	for (; inputIdx < length + _waveletData->Size; inputIdx += 2, currentHiLoIdx++)
	{
		// inputIdx - (input.Length - 1) is a count of cells that are overhang
		// PassSize - count of overhang cells is the max index for the j
		int maxInverseWaveletIndex = _waveletData->Size - (inputIdx - (length - 1));
		for (int j = 0; j < maxInverseWaveletIndex; j++)
		{
			int idxm = inputIdx - _waveletData->Size + j + 1;
			int idxms = _waveletData->Size - 1 - j;
			hi[currentHiLoIdx] += input[inputIdx - _waveletData->Size + j + 1] * _waveletData->HighPassDeconstruction[_waveletData->Size - 1 - j];
			lo[currentHiLoIdx] += input[inputIdx - _waveletData->Size + j + 1] * _waveletData->LowPassDeconstruction[_waveletData->Size - 1 - j];
		}
	}

	return WaveletLine(hi, lo, resultLength);
}

WaveletImage<pixel_t> FluxWaveletDenoising::Dwt2d(Matrix<pixel_t>& input)
{
	int coeffsWidth = this->GetDwtLength(input.Width(true), _waveletData->Size);
	int coeffsHeight = this->GetDwtLength(input.Height(true), _waveletData->Size);
	Matrix<pixel_t> lowHorizontal = Matrix<pixel_t>(coeffsWidth, input.Height());
	Matrix<pixel_t> hiHorizontal = Matrix<pixel_t>(coeffsWidth, input.Height());

	//Horizontal
#pragma omp parallel for
	for (int i = 0; i < input.Height(); i++)
	{
		std::vector<pixel_t> row = input.GetRow(i, true);
		WaveletLine dwt = Dwt(row.data(), row.size());
		lowHorizontal.SetRow(i, dwt.Lo);
		hiHorizontal.SetRow(i, dwt.Hi);
		dwt.Dispose();
		row.clear();
	}

	//std::cout << "Dwt H1\n";
	//hiHorizontal.Print();
	//std::cout << "Dwt L1\n";
	//lowHorizontal.Print();


	Matrix<pixel_t>* ca = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* ch = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* cv = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* cd = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);

	//Vertical
#pragma omp parallel for
	for (int i = 0; i < lowHorizontal.Width(); i++)
	{
		std::vector<pixel_t> columnLow = lowHorizontal.GetColumn(i, true);
		WaveletLine dwtLow = Dwt(columnLow.data(), columnLow.size());
		ca->SetColumn(i, dwtLow.Lo);
		ch->SetColumn(i, dwtLow.Hi);
		dwtLow.Dispose();
		columnLow.clear();

		std::vector<pixel_t> columnHi = hiHorizontal.GetColumn(i, true);
		WaveletLine dwtHi = Dwt(columnHi.data(), columnHi.size());
		cd->SetColumn(i, dwtHi.Hi);
		cv->SetColumn(i, dwtHi.Lo);
		columnHi.clear();
		dwtHi.Dispose();
	}

	lowHorizontal.Dispose();
	hiHorizontal.Dispose();

	WaveletImage<pixel_t> result = WaveletImage<pixel_t>();
	result.Width = coeffsWidth;
	result.Height = coeffsHeight;
	result.InitialWidth = input.Width();
	result.InitialHeight = input.Height();
	result.CV = cv;
	result.CH = ch;
	result.CA = ca;
	result.CD = cd;
	return result;
}


Matrix<pixel_t> FluxWaveletDenoising::Idwt2d(WaveletImage<pixel_t>& inputImage)
{
	// https://medium.com/@koushikc2000/2d-discrete-wavelet-transformation-and-its-applications-in-digital-image-processing-using-matlab-1f5c68672de3
	// https://pywavelets.readthedocs.io/en/latest/ref/2d-dwt-and-idwt.html

	int initialWidth = GetIdwtLength(inputImage.Width, _waveletData->Size);
	int initialHeight = GetIdwtLength(inputImage.Height, _waveletData->Size);

	Matrix<pixel_t> restoredH1 = Matrix<pixel_t>(inputImage.Width, initialHeight);
	Matrix<pixel_t> restoredL1 = Matrix<pixel_t>(inputImage.Width, initialHeight);

	//Vertical filtering
//#pragma omp parallel for
	for (int i = 0; i < inputImage.Width; i++)
	{
		std::vector<pixel_t> columnLow1 = inputImage.CV->GetColumn(i);
		std::vector<pixel_t> columnHi1 = inputImage.CD->GetColumn(i);
		WaveletLine line1 = WaveletLine(columnHi1.data(), columnLow1.data(), columnHi1.size());
		pixel_t* restoredHi = Idwt(line1);
		restoredH1.SetColumn(i, restoredHi);
		columnHi1.clear();
		columnLow1.clear();
		delete[] restoredHi;

		std::vector<pixel_t> columnLow2 = inputImage.CA->GetColumn(i);
		std::vector<pixel_t> columnHi2 = inputImage.CH->GetColumn(i);
		WaveletLine line2 = WaveletLine(columnHi2.data(), columnLow2.data(), columnHi2.size());
		pixel_t* restoredLo = Idwt(line2);
		restoredL1.SetColumn(i, restoredLo);
		columnHi2.clear();
		columnLow2.clear();
		delete[] restoredLo;
	}

	//std::cout << "H1\n";
	//restoredH1.Print();
	//std::cout << "L1\n";
	//restoredL1.Print();

	Matrix<pixel_t> restoredImage = Matrix<pixel_t>(inputImage.InitialWidth, inputImage.InitialHeight);

	//Horizontal filtering
//#pragma omp parallel for
	//for (int i = 0; i < restoredH1.Height(); i++)
	for (int i = 0; i < inputImage.InitialHeight; i++)
	{
		std::vector<pixel_t> lowRow = restoredL1.GetRow(i);
		std::vector<pixel_t> hiRow = restoredH1.GetRow(i);
		WaveletLine line = WaveletLine(hiRow.data(), lowRow.data(), lowRow.size());
		pixel_t* restored = Idwt(line);
		restoredImage.SetRow(i, restored);
		lowRow.clear();
		hiRow.clear();
		delete[] restored;
	}

	restoredH1.Dispose();
	restoredL1.Dispose();

	return restoredImage;
}

pixel_t* FluxWaveletDenoising::Idwt(WaveletLine& line)
{
	int initialLength = GetIdwtLength(line.Length, _waveletData->Size);
	pixel_t* result = new pixel_t[initialLength]{};

	int outputOffset = _waveletData->Size - 2;

	int len = (initialLength + _waveletData->Size - 2) / 2;

	for (int i = outputOffset / 2; i < len; i++)
	{
		for (int j = 0; j < _waveletData->Size / 2; j++)
		{
			int index = i - j;

			result[i * 2 - outputOffset] += line.Hi[index] * _waveletData->HighPassReconstruction[j * 2]
				+ line.Lo[index] * _waveletData->LowPassReconstruction[j * 2];

			result[i * 2 - outputOffset + 1] += line.Hi[index] * _waveletData->HighPassReconstruction[j * 2 + 1]
				+ line.Lo[index] * _waveletData->LowPassReconstruction[j * 2 + 1];
		}
	}
	return result;
}



FluxWaveletDenoising::~FluxWaveletDenoising()
{
	//if (_waveletData != nullptr) {
	//	delete _waveletData;
	//	_waveletData = nullptr;
	//}
}


std::vector<WaveletImage<pixel_t>> FluxWaveletDenoising::Wavedec(Matrix<pixel_t>& input) {


	std::vector<WaveletImage<pixel_t>> result = std::vector<WaveletImage<pixel_t>>();
	int depth = 1;
	Matrix<pixel_t>* currentMatrix = &input;

	while (currentMatrix->Width() >= _waveletData->Size && currentMatrix->Height() >= _waveletData->Size && depth < 7)
	{
		depth++;
		WaveletImage<pixel_t> dwt = Dwt2d(*currentMatrix);
		result.push_back(dwt);
		currentMatrix = dwt.CA;
	}

	return result;
}

Matrix<pixel_t> FluxWaveletDenoising::Waveinv(std::vector<WaveletImage<pixel_t>>& input) {

	
	WaveletImage<pixel_t>& currentImage = input.back();
	Matrix<pixel_t> recovered = Idwt2d(currentImage);
	WaveletImage<pixel_t> nextLevel = WaveletImage<pixel_t>();
	nextLevel.CA = &recovered;

	for (int i = input.size() - 2; i >= 0; --i)
	{
		WaveletImage<pixel_t>& toCopy = input[i];
		nextLevel.CD = toCopy.CD;
		nextLevel.CH = toCopy.CH;
		nextLevel.CV = toCopy.CV;
		nextLevel.Height = toCopy.Height;
		nextLevel.Width = toCopy.Width;
		nextLevel.InitialHeight = toCopy.InitialHeight;
		nextLevel.InitialWidth = toCopy.InitialWidth;

		Matrix<pixel_t> temp = Idwt2d(nextLevel);
		nextLevel.CA->Dispose();
		Matrix<pixel_t> recovered2 = temp;
		nextLevel.CA = &recovered2;
	}

	return *nextLevel.CA;
}


std::vector<WaveletImage<pixel_t>> FluxWaveletDenoising::ApplyDenoising(std::vector<WaveletImage<pixel_t>>& input, std::vector<pixel_t>& thresholdValues) {
	std::vector<WaveletImage<pixel_t>> result = std::vector<WaveletImage<pixel_t>>();
	result.reserve(input.size());

	for (int i = 0; i < input.size() && i < thresholdValues.size(); ++i)
	{

		WaveletImage<pixel_t> currentImage = input[i];
		WaveletImage<pixel_t> coppiedImage = currentImage.Copy();

		pixel_t threshold = thresholdValues[i];


		if (threshold != 0.0f) {
			ApplyThreshold(*coppiedImage.CA, threshold);
			ApplyThreshold(*coppiedImage.CD, threshold);
			ApplyThreshold(*coppiedImage.CH, threshold);
			ApplyThreshold(*coppiedImage.CV, threshold);
		}
		result.push_back(coppiedImage);
	}

	return result;
}

void FluxWaveletDenoising::ApplyThreshold(Matrix<pixel_t>& mat, pixel_t& threshold)
{
	int w = mat.Width();
	int h = mat.Height();

	pixel_t* currentPixels = mat.GetPointer(0);

	int inc = vfloat::size;
	vfloat thrVSqr = threshold * threshold;
	vfloat thrV = threshold * threshold;
	//Apply Garrote thesholding
	//https://github.com/PyWavelets/pywt/blob/4591748823f2259844b6b3de0ddf60d1a85a6fa7/pywt/_thresholding.py
	//https://github.com/PyWavelets/pywt/pull/354
	//https://pywavelets.readthedocs.io/en/latest/ref/thresholding-functions.html#r94bc4984de23-2

	//thresholded = (1 - value **2 / magnitude **2)
	//x* (1 - (value / abs(x)) **2)

#pragma omp parallel for
	for (int y = 0; y < h; ++y)
	{
		int currentLine = y * w;
		for (int x = 0; x < w - inc; x += inc)
		{
			int index = currentLine + x;
			vfloat values = vfloat::load_aligned(&currentPixels[index]);
			vfloat absvalues = xsimd::abs(values);
			vfloat nn_garrote = values * (1 - (thrVSqr / absvalues * absvalues));
			auto maskThr = xsimd::ge(absvalues, thrV);
			values = xsimd::select(maskThr, nn_garrote, _zeroV);
			values.store_aligned(&currentPixels[index]);
			
		}

		for (int x = w - inc; x < w; x++)
		{
			int index = currentLine + x;
			pixel_t values = currentPixels[index];
			pixel_t nnGarrote = values * (1 - (threshold * threshold / values * values));
			if (std::abs(values) <= threshold || values == 0) {
				values = 0;
			}
			else {
				values = nnGarrote;
			}
			currentPixels[index] = values;
		}
	}

}