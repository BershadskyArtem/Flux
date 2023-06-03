#include "WaveletComposeImageOperation.h"

ProcessingCacheEntry* WaveletComposeImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	//Suppose we have valid wavelet decomposition in previous cached stage 

	//Delete useless cache 

	if (currentCachedStage->Caches != nullptr) {
		InternalLabImage* labToDelete = (InternalLabImage*)currentCachedStage->Caches;
		delete[] labToDelete->LPixels;
		delete[] labToDelete->APixels;
		delete[] labToDelete->BPixels;
		delete labToDelete;
		currentCachedStage->Caches = nullptr;
	}
	InternalLabImage* result = new InternalLabImage();

	WaveletCache* prevCache = (WaveletCache*)previousCachedStage->Caches;

	Daubechies4* db4 = new Daubechies4();

	FluxWaveletDenoising denoiser = FluxWaveletDenoising(db4);

	//std::vector<std::vector<WaveletImage<pixel_t>>> arrIn = std::vector<std::vector<WaveletImage<pixel_t>>>();
	//std::vector<Matrix<pixel_t>> arrOut = std::vector<Matrix<pixel_t>>();
	//
	//arrIn.push_back(*prevCache->LDec);
	//arrIn.push_back(*prevCache->ADec);
	//arrIn.push_back(*prevCache->BDec);
	//arrOut.resize(3);
	//
	////Dirty gives about 30% boost in speed
	//#pragma omp parallel for
	//for (int i = 0; i < 3; i++)
	//{
	//	arrOut[i] = denoiser.Waveinv(arrIn[i]);
	//}

	//arrIn[0] = &prevCache->LDec;

	Matrix<pixel_t> lMat = denoiser.Waveinv(*prevCache->LDec);
	Matrix<pixel_t> aMat = denoiser.Waveinv(*prevCache->ADec);
	Matrix<pixel_t> bMat = denoiser.Waveinv(*prevCache->BDec);

	//Matrix<pixel_t> lMat = arrOut[0];
	//Matrix<pixel_t> aMat = arrOut[1];
	//Matrix<pixel_t> bMat = arrOut[2];


	result->LPixels = lMat.GetPointer();
	result->APixels = aMat.GetPointer();
	result->BPixels = bMat.GetPointer();
	result->Width = lMat.Width();
	result->Height = lMat.Height();


	currentCachedStage->Caches = result;
	denoiser.Dispose();
	delete db4;

	return currentCachedStage;
}

void WaveletComposeImageOperation::Dispose()
{
}

void WaveletComposeImageOperation::DisposeCacheEntry(ProcessingCacheEntry* cache)
{
	if (cache->Caches == nullptr)
		return;
	InternalLabImage* labToDelete = (InternalLabImage*)cache->Caches;
	delete[] labToDelete->LPixels;
	delete[] labToDelete->APixels;
	delete[] labToDelete->BPixels;
	delete labToDelete;
	cache->Caches = nullptr;
	cache->CachesCount = 0;
}
