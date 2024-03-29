#pragma once
#include "../BaseImageOperation.h"
#include "../../../infrastructure/FLXDefines.h"
#include "../../FluxWaveletDenoising.h"
#include "../../Wavelets/Daubechies4.h"
#include "../../../infrastructure/FluxMath.h"

class DenoiseImageOperation : public BaseImageOperation {
protected:
	static std::vector<pixel_t> s_DenoisingLuminanceThresholds;
	static std::vector<pixel_t> s_DenoisingChrominanceThresholds;

public:

	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;

	// Inherited via BaseImageOperation
	virtual void Dispose() override;

	// Inherited via BaseImageOperation
	virtual void DisposeCacheEntry(ProcessingCacheEntry* cache) override;
};