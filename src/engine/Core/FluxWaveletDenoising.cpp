#include "FluxWaveletDenoising.h"
#include <iostream>

FluxWaveletDenoising::FluxWaveletDenoising(WaveletData* data)
{
	_waveletData = data;
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
			hi[currentHiLoIdx] += input[inputIdx - _waveletData->Size + j + 1] * _waveletData->HighPassDeconstruction[_waveletData->Size - 1 - j];
			lo[currentHiLoIdx] += input[inputIdx - _waveletData->Size + j + 1] * _waveletData->LowPassDeconstruction[_waveletData->Size - 1 - j];
		}
	}

	return WaveletLine(hi, lo, resultLength);
}



WaveletImage<pixel_t> FluxWaveletDenoising::Dwt2d(Matrix<pixel_t>& input)
{
	int coeffsWidth = this->GetDwtLength(input.Width(), _waveletData->Size);
	int coeffsHeight = this->GetDwtLength(input.Height(), _waveletData->Size);
	Matrix<pixel_t> lowHorizontal = Matrix<pixel_t>(coeffsWidth, input.Height());
	Matrix<pixel_t> hiHorizontal = Matrix<pixel_t>(coeffsWidth, input.Height());

	Matrix<pixel_t>* ll = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* hl = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* lh = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);
	Matrix<pixel_t>* hh = new Matrix<pixel_t>(coeffsWidth, coeffsHeight);

//#pragma omp parallel for 
	for (int y = 0; y < input.Height(); y++)
	{
		std::vector<pixel_t> row = input.GetRow(y);
		WaveletLine filteredRow = this->Dwt(&row[0], input.Width());
		row.clear();
		lowHorizontal.SetRow(y, filteredRow.Lo);
		hiHorizontal.SetRow(y, filteredRow.Hi);
	}

//Dwt on columns
//#pragma omp parallel for 
	for (int x = 0; x < input.Width(); x++)
	{
		std::vector<pixel_t> columnLow = lowHorizontal.GetColumn(x);
		WaveletLine filteredColumnLow = this->Dwt(&columnLow[0], input.Height());
		columnLow.clear();
	
		std::vector<pixel_t> columnHi = hiHorizontal.GetColumn(x);
		WaveletLine filteredColumnHi = this->Dwt(&columnHi[0], input.Height());
		columnHi.clear();

		ll->SetColumn(x, filteredColumnHi.Lo);
		hl->SetColumn(x, filteredColumnHi.Hi);
		lh->SetColumn(x, filteredColumnLow.Lo);
		hh->SetColumn(x, filteredColumnLow.Hi);
	}
	
	lowHorizontal.Dispose();
	hiHorizontal.Dispose();

	WaveletImage<pixel_t> result = WaveletImage<pixel_t>();
	result.Width = coeffsWidth;
	result.Height = coeffsHeight;
	result.CV = hh;
	result.CH = ll;
	result.CA = lh;
	result.CD = hl;
	return result;
}


Matrix<pixel_t> FluxWaveletDenoising::Idwt2d(WaveletImage<pixel_t> &inputImage)
{
	// https://medium.com/@koushikc2000/2d-discrete-wavelet-transformation-and-its-applications-in-digital-image-processing-using-matlab-1f5c68672de3

	int initialWidth = GetIdwtLength(inputImage.Width, _waveletData->Size);
	int initialHeight = GetIdwtLength(inputImage.Height, _waveletData->Size);
	Matrix<pixel_t> restoredH1 = Matrix<pixel_t>(inputImage.Width, initialHeight);
	Matrix<pixel_t> restoredL1 = Matrix<pixel_t>(inputImage.Width, initialHeight);

	for (int i = 0; i < inputImage.Width; i++)
	{
		std::vector<pixel_t> lowColumn = inputImage.CH->GetColumn(i);
		std::vector<pixel_t> hiColumn = inputImage.CD->GetColumn(i);
		WaveletLine line = WaveletLine(&hiColumn[0], &lowColumn[0], lowColumn.size());
		pixel_t* restoredH1Column = Idwt(line);
		restoredH1.SetColumn(i, restoredH1Column);
		lowColumn.clear();
		hiColumn.clear();


		std::vector<pixel_t> lowColumn2 = inputImage.CA->GetColumn(i);
		std::vector<pixel_t> hiColumn2 = inputImage.CV->GetColumn(i);
		WaveletLine line2 = WaveletLine(&hiColumn2[0], &lowColumn2[0], lowColumn2.size());
		pixel_t* restoredL1Column = Idwt(line2);
		restoredL1.SetColumn(i, restoredL1Column);
		lowColumn2.clear();
		hiColumn2.clear();
	}

	Matrix<pixel_t> restoredImage = Matrix<pixel_t>(initialWidth, initialHeight);

	for (int i = 0; i < inputImage.Height; i++)
	{
		std::vector lowColumn = restoredL1.GetRow(i);
		std::vector hiColumn = restoredH1.GetRow(i);

		WaveletLine line = WaveletLine(&hiColumn[0], &lowColumn[0], lowColumn.size());
		pixel_t* restoredLine = Idwt(line);
		restoredImage.SetRow(i, restoredLine);
		lowColumn.clear();
		hiColumn.clear();
	}

	return restoredImage;
}

pixel_t* FluxWaveletDenoising::Idwt(WaveletLine& line)
{
	int initialLength = GetIdwtLength(line.Length, _waveletData->Size);
	pixel_t* result = new pixel_t[initialLength];

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
	if (_waveletData != nullptr) {
		delete _waveletData;
		_waveletData = nullptr;
	}
}
