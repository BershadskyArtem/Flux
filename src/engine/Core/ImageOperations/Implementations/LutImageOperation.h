#pragma once
#include "../BaseImageOperation.h"
#include "../../../Color/Converter.h"
#include "../../../Color/ColorLUTConverter.h"
#include "../../../infrastructure/LUTf3d.h"
#include "../../../infrastructure/FluxMath.h"

class LutImageOperation : BaseImageOperation {
	// Inherited via BaseImageOperation
	pixel_t GetBrightnessUsingColorMask(pixel_t l, pixel_t a, pixel_t b, HSLColorProcessingSettings& settings);
	void ElevateBrightness(pixel_t l, pixel_t a, pixel_t b, LUTProcessingSettings &settings);
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;
};