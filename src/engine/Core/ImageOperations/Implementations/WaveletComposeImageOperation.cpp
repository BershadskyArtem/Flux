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

	Matrix<pixel_t> lMat = denoiser.Waveinv(*prevCache->LDec);
	Matrix<pixel_t> aMat = denoiser.Waveinv(*prevCache->ADec);
	Matrix<pixel_t> bMat = denoiser.Waveinv(*prevCache->BDec);



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
