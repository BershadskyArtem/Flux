#include "LutImageOperation.h"

pixel_t LutImageOperation::GetBrightnessUsingColorMask(pixel_t l, pixel_t a, pixel_t b, HSLColorProcessingSettings& settings)
{
	pixel_t result = 0.0f;
	settings.


	return pixel_t();
}

void LutImageOperation::ElevateBrightness(pixel_t l, pixel_t a, pixel_t b, LUTProcessingSettings& settings)
{
	pixel_t elevateAmount = 0.f;
	pixel_t chrominance = 0.f;
	pixel_t hue = 0.f;
	pixel_t luma = 0.f;
	pixel_t currentLuma = 0.f;

	Converter::OkLab2OkLCh(l, a, b, luma, chrominance, hue);

	//Exposure
	currentLuma = luma * std::pow(2, settings.LightSettings.Exposure);
	//Then brightness
	float brightness = settings.LightSettings.Brightness / 100.f;
	brightness = 1 - brightness;
	currentLuma = std::pow(currentLuma, brightness);
	//Then contrast
	pixel_t contrast = settings.LightSettings.Contrast / 100.f;
	pixel_t contrastCurveAtPoint = FluxMath::ContrastSmoothstep(currentLuma);
	FluxMath::LinearInterpolate(contrast, contrastCurveAtPoint, currentLuma);
	settings.HSLSettings.
	//HSL
	pixel_t GetBrightnessUsingColorMask(pixel_t l, pixel_t a, pixel_t b, settings);








}

ProcessingCacheEntry* LutImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	//No need to delete useless current cache (Internal lab image) because we can populate it with valid new data
	if (currentCachedStage == nullptr) {
		currentCachedStage = new ProcessingCacheEntry();
	}
	if (currentCachedStage->Caches == nullptr) {
		currentCachedStage->Caches = new InternalLabImage();
		currentCachedStage->CachesCount = 1;
	}
	//Refill cache
	LUTf3d* lut = new LUTf3d();
	lut->Init(LUT_DIM, LUT_DIM, LUT_DIM);

	LUTProcessingSettings& settings = newSettings->LUT;

	for (pixel_t l = 0; l < LUT_DIM; l++)
	{
		for (pixel_t a = 0; a < LUT_DIM; a++)
		{
			for (pixel_t b = 0; b < LUT_DIM; b++)
			{
				pixel_t lF = l / LUT_DIM;
				pixel_t aF = a / LUT_DIM;
				pixel_t bF = b / LUT_DIM;

				ElevateBrightness(lF, aF, bF, settings);
			}
		}
	}


	/*LUT
		WB
		Exposure
		Brightness
		HDR
		Contrast
		Levels
		Curve
		Tonning


	*/









	return nullptr;
}

void LutImageOperation::Dispose()
{
}
