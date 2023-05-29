#include "DenoiseImageOperation.h"
#include <iostream>

std::vector<pixel_t> DenoiseImageOperation::s_DenoisingLuminanceThresholds = { 0.1f, 0.15f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
//std::vector<pixel_t> DenoiseImageOperation::s_DenoisingChrominanceThresholds = { 0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.75f, 0.5f, 0.0f, 0.0f };
//std::vector<pixel_t> DenoiseImageOperation::s_DenoisingChrominanceThresholds = { 0.0f, 0.25f, 0.5f, 0.5f, 0.5f, 0.35f, 0.5f, 0.0f, 0.0f };
std::vector<pixel_t> DenoiseImageOperation::s_DenoisingChrominanceThresholds = { 0.0f, 0.25f, 0.5f, 0.5f, 0.5f, 0.45f, 0.5f, 0.0f, 0.0f };
//std::vector<pixel_t> DenoiseImageOperation::s_DenoisingChrominanceThresholds = { 0.05f,0.25f,0.9f,1.8f, 6.0f };

ProcessingCacheEntry* DenoiseImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	pixel_t luminanceDenoising = newSettings->Denoise.Luminance / 100.f;
	pixel_t chrominanceDenoising = newSettings->Denoise.Chrominance / 100.f;

	//Copy and adjust denoising thresholds 
	std::vector<pixel_t> copiedLuminanceThresholds = std::vector<pixel_t>();
	std::vector<pixel_t> copiedChrominanceThresholds = std::vector<pixel_t>();
	copiedLuminanceThresholds.reserve(s_DenoisingLuminanceThresholds.size());
	copiedChrominanceThresholds.reserve(s_DenoisingChrominanceThresholds.size());

	for (pixel_t i = 0; i < s_DenoisingChrominanceThresholds.size(); i++)
	{
		copiedLuminanceThresholds.push_back(FluxMath::LinearInterpolate(luminanceDenoising, s_DenoisingLuminanceThresholds[i], 0.f));
		copiedChrominanceThresholds.push_back(FluxMath::LinearInterpolate(chrominanceDenoising,  s_DenoisingChrominanceThresholds[i], 0.f));
	}

	WaveletCache* denoiseCache = (WaveletCache*)currentCachedStage->Caches;

	//Dispose cache
	if (denoiseCache != nullptr) {
		if (denoiseCache->LDec != nullptr) {
			for (int i = 0; i < denoiseCache->LDec->size(); i++) {
				denoiseCache->LDec->at(i).Dispose();
			}
			denoiseCache->LDec->clear();
			denoiseCache->LDec = nullptr;
		}

		if (denoiseCache->ADec != nullptr) {
			for (int i = 0; i < denoiseCache->ADec->size(); i++) {
				denoiseCache->ADec->at(i).Dispose();
			}
			denoiseCache->ADec->clear();
			denoiseCache->ADec = nullptr;
		}

		if (denoiseCache->BDec != nullptr) {
			for (int i = 0; i < denoiseCache->BDec->size(); i++) {
				denoiseCache->BDec->at(i).Dispose();
			}
			denoiseCache->BDec->clear();
			denoiseCache->BDec = nullptr;
		}

		delete denoiseCache;
		denoiseCache = nullptr;
	}

	//Assume we have valid wavelet transform in previousCachedStage
	WaveletCache* cache = (WaveletCache*)previousCachedStage->Caches;
	std::cout << "Applying denoising for image. Depth is " << cache->ADec->size() << std::endl;
	std::cout << "Thresholds for chrominance: " << std::endl;

	for (int i = 0; i < copiedChrominanceThresholds.size(); i++)
	{
		std::cout << i << " = " << copiedChrominanceThresholds[i] << std::endl;
	}

	std::vector<WaveletImage<pixel_t>>* denoisedL = FluxWaveletDenoising::ApplyDenoising(*cache->LDec, copiedLuminanceThresholds);
	std::vector<WaveletImage<pixel_t>>* denoisedA = FluxWaveletDenoising::ApplyDenoising(*cache->ADec, copiedChrominanceThresholds);
	std::vector<WaveletImage<pixel_t>>* denoisedB = FluxWaveletDenoising::ApplyDenoising(*cache->BDec, copiedChrominanceThresholds);

	denoiseCache = new WaveletCache();
	denoiseCache->LDec = denoisedL;
	denoiseCache->ADec = denoisedA;
	denoiseCache->BDec = denoisedB;

	currentCachedStage->Caches = denoiseCache;

	return currentCachedStage;
}

void DenoiseImageOperation::Dispose()
{
}
