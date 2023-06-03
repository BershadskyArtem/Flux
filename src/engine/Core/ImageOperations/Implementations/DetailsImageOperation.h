#pragma once
#include <vector>
#include <algorithm>
#include <xsimd/xsimd.hpp>
#include "../../../infrastructure/FLXDefines.h"
#include "../BaseImageOperation.h"
#include "../../../infrastructure/FluxMath.h"

class DetailsImageOperation : public BaseImageOperation {
private:
	WaveletImage<pixel_t>* ApplyToLevel(WaveletImage<pixel_t> &image, pixel_t value);
	WaveletImage<pixel_t>* UnsharpMaskToLevel(WaveletImage<pixel_t> &image, int level, pixel_t value);
	Matrix<pixel_t>* UnsharpMaskToMatrix(Matrix<pixel_t> &image, int level, pixel_t value);

protected:
	static std::vector<pixel_t> s_SharpenMultipliers;
	static std::vector<pixel_t> s_TextureMultipliers;

public:
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;

	// Inherited via BaseImageOperation
	virtual void Dispose() override;

	// Inherited via BaseImageOperation
	virtual void DisposeCacheEntry(ProcessingCacheEntry* cache) override;

};