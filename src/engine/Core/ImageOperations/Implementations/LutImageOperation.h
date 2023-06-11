#pragma once
#include "../BaseImageOperation.h"
#include "../../../Color/Converter.h"
#include "../../../Color/ColorLUTConverter.h"
#include "../../../infrastructure/LUTf3d.h"
#include "../../../infrastructure/FluxMath.h"
#include "../../../io/Encoders/JpegImageEncoder.h"
#include "../../../Color/Colors.h"

class LutImageOperation : public BaseImageOperation {
	// Inherited via BaseImageOperation
	pixel_t GetBrightnessUsingColorMask(pixel_t l, pixel_t c, pixel_t h, HSLColorProcessingSettings& settings);
	pixel_t ElevateBrightness(const OkLChColor &color, LUTProcessingSettings &settings);
	pixel_t ElevateHue(const OkLChColor& color, LUTProcessingSettings &settings);
	pixel_t ElevateSaturation(const OkLChColor& color, LUTProcessingSettings &settings);
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;

	// Inherited via BaseImageOperation
	virtual void DisposeCacheEntry(ProcessingCacheEntry* cache) override;
};