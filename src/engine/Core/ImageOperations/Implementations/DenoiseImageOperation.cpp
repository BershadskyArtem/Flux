#include "DenoiseImageOperation.h"


std::vector<pixel_t> DenoiseImageOperation::s_DenoisingLuminanceThresholds = { 1,1,1,1 };
std::vector<pixel_t> DenoiseImageOperation::s_DenoisingChrominanceThresholds = { 1,1,1,1 };

ProcessingCacheEntry* DenoiseImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
    int luminanceDenoising = newSettings->Denoise.Luminance;
    int chrominanceDenoising = newSettings->Denoise.Chrominance;
    
    //Copy and adjust denoising thresholds 
    std::vector<pixel_t> copiedLuminanceThresholds = std::vector<pixel_t>(s_DenoisingLuminanceThresholds);

    for (int i = 0; i < copiedLuminanceThresholds.size(); i++)
    {
        copiedLuminanceThresholds[i] *= luminanceDenoising;
    }

    std::vector<pixel_t> copiedChrominanceThresholds = std::vector<pixel_t>(s_DenoisingChrominanceThresholds);

    for (int i = 0; i < copiedChrominanceThresholds.size(); i++)
    {
        copiedChrominanceThresholds[i] *= chrominanceDenoising;
    }
    
    DenoiseCache* denoiseCache = (DenoiseCache*)currentCachedStage->Caches;

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

    std::vector<WaveletImage<pixel_t>>* denoisedL = FluxWaveletDenoising::ApplyDenoising(*cache->LDec, copiedLuminanceThresholds);
    std::vector<WaveletImage<pixel_t>>* denoisedA = FluxWaveletDenoising::ApplyDenoising(*cache->ADec, copiedChrominanceThresholds);
    std::vector<WaveletImage<pixel_t>>* denoisedB = FluxWaveletDenoising::ApplyDenoising(*cache->BDec, copiedChrominanceThresholds);

    denoiseCache = new DenoiseCache();
    denoiseCache->LDec = denoisedL;
    denoiseCache->ADec = denoisedA;
    denoiseCache->BDec = denoisedB;
    
    currentCachedStage->Caches = denoiseCache;

    return currentCachedStage;
}
