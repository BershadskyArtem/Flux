#pragma once
#include <array>
#include <vector>
#include "../BaseImageOperation.h"
#include "../../../infrastructure/FLXDefines.h"
#include "../../FluxWaveletDenoising.h"
#include "../../Wavelets/Daubechies4.h"
#include "../../../infrastructure/PixelsHelper.h"

class WaveletComposeImageOperation : public BaseImageOperation {
	// Inherited via BaseImageOperation
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;
};