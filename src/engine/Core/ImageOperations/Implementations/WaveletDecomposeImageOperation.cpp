#include "WaveletDecomposeImageOperation.h"

ProcessingCacheEntry* WaveletDecomposeImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
    //Delete current cache values 
    WaveletCache* cacheToDelete = (WaveletCache*)currentCachedStage->Caches;
    
    if (cacheToDelete != nullptr) {
        for (int i = 0; i < cacheToDelete->LDec->size(); i++) {
            cacheToDelete->LDec->at(i).Dispose();
            cacheToDelete->ADec->at(i).Dispose();
            cacheToDelete->BDec->at(i).Dispose();
        }

        cacheToDelete->LDec->clear();
        cacheToDelete->ADec->clear();
        cacheToDelete->BDec->clear();

        delete cacheToDelete->LDec;
        delete cacheToDelete->ADec;
        delete cacheToDelete->BDec;
        delete cacheToDelete;

        cacheToDelete = nullptr;
    }

    //Wavelet decompose 
    InternalLabImage* labImage = (InternalLabImage*)previousCachedStage->Caches;
    Daubechies4* db4 = new Daubechies4();

    FluxWaveletDenoising denoiser = FluxWaveletDenoising(db4);
    
    Matrix<pixel_t> matL = Matrix<pixel_t>(labImage->Width, labImage->Height, labImage->LPixels);
    Matrix<pixel_t> matA = Matrix<pixel_t>(labImage->Width, labImage->Height, labImage->APixels);
    Matrix<pixel_t> matB = Matrix<pixel_t>(labImage->Width, labImage->Height, labImage->BPixels);

    WaveletCache* waveletCache = new WaveletCache();


    std::vector<WaveletImage<pixel_t>>* lD = denoiser.Wavedec(matL);;
    std::vector<WaveletImage<pixel_t>>* aD = denoiser.Wavedec(matA);;
    std::vector<WaveletImage<pixel_t>>* bD = denoiser.Wavedec(matB);;
    waveletCache->LDec = lD;
    waveletCache->ADec = aD;
    waveletCache->BDec = bD;
    
    currentCachedStage->Caches = waveletCache;

    return currentCachedStage;
}

void WaveletDecomposeImageOperation::Dispose()
{
}
