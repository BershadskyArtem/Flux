#include "LutImageOperation.h"
#include "../../../Color/FluxColorMath.h"
#include "../../../infrastructure/BenchmarkHelper.h"

pixel_t LutImageOperation::GetBrightnessUsingColorMask(pixel_t l, pixel_t c, pixel_t h, HSLColorProcessingSettings& settings)
{
	return FluxColorMath::IsInsideHueChrominanceBoundary(h, c, settings.SelectedColor);
}

/// <summary>
/// Applies HSL luminocity, HDR, exp, brightness and contrast.
/// </summary>
/// <param name="l"></param>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="settings"></param>
/// <returns></returns>
pixel_t LutImageOperation::ElevateBrightness(const OkLChColor& color, LUTProcessingSettings& settings)
{
	const pixel_t divisor = 250.f;
	pixel_t elevateAmount = 0.f;
	pixel_t chrominance = color.C;
	pixel_t hue = color.H;
	pixel_t luma = color.L;
	pixel_t currentLuma = 0.f;

	//Converter::OkLab2OkLCh(l, a, b, luma, chrominance, hue);

	//Exposure
	currentLuma = luma * std::pow(2, settings.LightSettings.Exposure);
	//Then brightness
	float brightness = settings.LightSettings.Brightness / divisor;
	
	//Gamma = brightness
	// 
	//currentLuma = std::pow(currentLuma, brightness);
	// 
	//Then contrast

	pixel_t contrast = settings.LightSettings.Contrast / divisor;
	pixel_t contrastCurveAtPoint = FluxMath::ContrastSmoothstep(currentLuma);
	currentLuma = FluxMath::LinearInterpolate(contrast, contrastCurveAtPoint, currentLuma);

	pixel_t totalElevation = 1.f;
	
	pixel_t whFactor = FluxMath::DualSmoothstep(luma, 1.f, 0.9f, -0.85f);
	pixel_t hiFactor = FluxMath::DualSmoothstep(luma, 1.f, 0.9f, -0.75f);
	pixel_t blFactor = FluxMath::DualSmoothstep(luma, 0.f, 0.4f, -0.37f);
	pixel_t shFactor = FluxMath::DualSmoothstep(luma, 0.f, 0.5f, -0.45f);

	totalElevation -= whFactor * settings.HDRSettings.Whites / divisor +
		blFactor * settings.HDRSettings.Blacks / divisor +
		hiFactor * settings.HDRSettings.Highlights / divisor +
		shFactor * settings.HDRSettings.Shadows / divisor;

	//settings.HSLSettings.
	//HSL corrections 
	pixel_t colorFactorr = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Red);
	pixel_t colorFactoro = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Orange);
	pixel_t colorFactory = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Yellow);
	pixel_t colorFactorg = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Green);
	pixel_t colorFactora = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Aqua);
	pixel_t colorFactorb = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Blue);
	pixel_t colorFactorm = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Magenta);
	pixel_t colorFactorp = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Purple);

	totalElevation -= brightness + colorFactorr * settings.HSLSettings.Red.Lightness / divisor +
		colorFactoro * settings.HSLSettings.Orange.Lightness / divisor +
		colorFactory * settings.HSLSettings.Yellow.Lightness / divisor +
		colorFactorg * settings.HSLSettings.Green.Lightness / divisor +
		colorFactora * settings.HSLSettings.Aqua.Lightness / divisor +
		colorFactorb * settings.HSLSettings.Blue.Lightness / divisor +
		colorFactorm * settings.HSLSettings.Magenta.Lightness / divisor +
		colorFactorp * settings.HSLSettings.Purple.Lightness / divisor;

	for (int i = 0; i < settings.HSLSettings.CustomColorsCount; i++)
	{
		totalElevation -= GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.CustomColors[i]) 
			* settings.HSLSettings.CustomColors[i].Lightness / divisor;
	}

	totalElevation = std::max(totalElevation, 0.0001f);
	totalElevation = std::min(totalElevation, 1000.f);
	
	currentLuma = std::pow(currentLuma, totalElevation);
	
	return currentLuma;
}

pixel_t LutImageOperation::ElevateHue(const OkLChColor& color, LUTProcessingSettings& settings)
{
	pixel_t elevateAmount = 0.f;
	pixel_t chrominance = color.C;
	pixel_t hue = color.H;
	pixel_t luma = color.L;
	pixel_t currentHue = 0.f;

	//Converter::OkLab2OkLCh(l, a, b, luma, chrominance, hue);


	pixel_t colorFactorr = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Red);
	pixel_t colorFactoro = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Orange);
	pixel_t colorFactory = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Yellow);
	pixel_t colorFactorg = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Green);
	pixel_t colorFactora = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Aqua);
	pixel_t colorFactorb = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Blue);
	pixel_t colorFactorm = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Magenta);
	pixel_t colorFactorp = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Purple);

	pixel_t totalElevation = 0;

	totalElevation += colorFactorr * settings.HSLSettings.Red.HueShift / (2 * PI) +
		colorFactoro * settings.HSLSettings.Orange.HueShift / (2 * PI) +
		colorFactory * settings.HSLSettings.Yellow.HueShift / (2 * PI) +
		colorFactorg * settings.HSLSettings.Green.HueShift / (2 * PI) +
		colorFactora * settings.HSLSettings.Aqua.HueShift / (2 * PI) +
		colorFactorb * settings.HSLSettings.Blue.HueShift / (2 * PI) +
		colorFactorm * settings.HSLSettings.Magenta.HueShift / (2 * PI) +
		colorFactorp * settings.HSLSettings.Purple.HueShift / (2 * PI);

	for (int i = 0; i < settings.HSLSettings.CustomColorsCount; i++)
	{
		totalElevation += GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.CustomColors[i])
			* settings.HSLSettings.CustomColors[i].HueShift / (2 * PI);
	}

	currentHue = hue + totalElevation;
	return currentHue;

}

pixel_t LutImageOperation::ElevateSaturation(const OkLChColor& color, LUTProcessingSettings& settings)
{
	pixel_t luma = color.L;
	pixel_t chrominance = color.C;
	pixel_t hue = color.H;

	//Converter::OkLab2OkLCh(l, a, b, luma, chrominance, hue);

	pixel_t colorFactorr = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Red);
	pixel_t colorFactoro = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Orange);
	pixel_t colorFactory = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Yellow);
	pixel_t colorFactorg = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Green);
	pixel_t colorFactora = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Aqua);
	pixel_t colorFactorb = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Blue);
	pixel_t colorFactorm = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Magenta);
	pixel_t colorFactorp = GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.Purple);
	
	pixel_t vibranceFactor = FluxMath::DualSmoothstep(chrominance, 0.1f, 0.1f, -0.1f);
	
	pixel_t chrominanceElevation = 1;
	const pixel_t divisor = 100.0f;

	chrominanceElevation += (settings.BasicColorSettings.Vibrance / divisor) * vibranceFactor +
		colorFactorr * settings.HSLSettings.Red.Saturation / divisor +
		colorFactoro * settings.HSLSettings.Orange.Saturation / divisor +
		colorFactory * settings.HSLSettings.Yellow.Saturation / divisor +
		colorFactorg * settings.HSLSettings.Green.Saturation / divisor +
		colorFactora * settings.HSLSettings.Aqua.Saturation / divisor +
		colorFactorb * settings.HSLSettings.Blue.Saturation / divisor +
		colorFactorm * settings.HSLSettings.Magenta.Saturation / divisor +
		colorFactorp * settings.HSLSettings.Purple.Saturation / divisor;

	for (int i = 0; i < settings.HSLSettings.CustomColorsCount; i++)
	{
		chrominanceElevation += GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.CustomColors[i])
			* settings.HSLSettings.CustomColors[i].Saturation / divisor;
	}

	return std::clamp(chrominanceElevation * chrominance, 0.0f, 0.5f);
}


/// <summary>
/// Applies 3d LUT to and image
/// </summary>
/// <param name="previousCachedStage">Must be type of InternalLabImage. Lab needs to be converted from RGB color space</param>
/// <param name="currentCachedStage"></param>
/// <param name="newSettings"></param>
/// <returns>Cache with InternalImageData inside it. sRGB.</returns>
ProcessingCacheEntry* LutImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	//No need to delete useless current cache (Internal lab image) because we can populate it with valid new data

	//Internal lab image
	InternalImageData* currentCache = (InternalImageData*)currentCachedStage->Caches;
	InternalLabImage* previousCache = (InternalLabImage*)previousCachedStage->Caches;

	int w = previousCache->Width;
	int h = previousCache->Height;

	if (currentCache == nullptr) {
		currentCache = new InternalImageData();
		currentCache->Width = previousCache->Width;
		currentCache->Height = previousCache->Height;
		currentCache->RPixels = new pixel_t[w * h];
		currentCache->GPixels = new pixel_t[w * h];
		currentCache->BPixels = new pixel_t[w * h];
	}
	else {
		if (currentCache->Width != w || currentCache->Height != h) {
			//Invalidate caches
			if (currentCache->RPixels != nullptr) {
				delete[] currentCache->RPixels;
			}
			if (currentCache->GPixels != nullptr) {
				delete[] currentCache->GPixels;
			}
			if (currentCache->BPixels != nullptr) {
				delete[] currentCache->BPixels;
			}
			currentCache->Width = previousCache->Width;
			currentCache->Height = previousCache->Height;
			currentCache->RPixels = new pixel_t[w * h];
			currentCache->GPixels = new pixel_t[w * h];
			currentCache->BPixels = new pixel_t[w * h];
		}
	}

	//Refill cache
	LUTf3d* lutL = new LUTf3d();
	lutL->Init(LUT_DIM, LUT_DIM, LUT_DIM);

	LUTf3d* lutA = new LUTf3d();
	lutA->Init(LUT_DIM, LUT_DIM, LUT_DIM);

	LUTf3d* lutB = new LUTf3d();
	lutB->Init(LUT_DIM, LUT_DIM, LUT_DIM);

	LUTProcessingSettings& settings = newSettings->LUT;

	for (pixel_t r = 0; r < LUT_DIM; r++)
	{
		for (pixel_t g = 0; g < LUT_DIM; g++)
		{
			for (pixel_t b = 0; b < LUT_DIM; b++)
			{
				//Input
				RGBColor inColor = RGBColor(r / LUT_DIM, g / LUT_DIM, b / LUT_DIM);
				//Convert
				OkLChColor inlchColor = inColor.ToOkLCh();

				//Evaluate
				pixel_t lPixFinal = ElevateBrightness(inlchColor, settings);
				pixel_t hPixFinal = ElevateHue(inlchColor, settings);
				//pixel_t hPixFinal = inlchColor.H;
				pixel_t cPixFinal = ElevateSaturation(inlchColor, settings);

				//Convert back
				OkLChColor outLChColor = OkLChColor(lPixFinal, cPixFinal, hPixFinal);
				sRGBColor outColor = outLChColor.TosRGB();

				//Output
				lutL->SetValue(outColor.R, r, g, b);
				lutA->SetValue(outColor.G, r, g, b);
				lutB->SetValue(outColor.B, r, g, b);
			}
		}
	}

	pixel_t* rPix = new pixel_t[w * h]{};
	pixel_t* gPix = new pixel_t[w * h]{};
	pixel_t* bPix = new pixel_t[w * h]{};

	Converter::OKLab2RGB(previousCache->LPixels, previousCache->APixels, previousCache->BPixels, rPix, gPix, bPix, w, h);

	//Apply 3D luts 
	LUTf3d::ApplyLUTs(rPix, gPix, bPix, currentCache->RPixels, currentCache->GPixels, currentCache->BPixels, *lutL, *lutA, *lutB, w, h);

	delete[] rPix;
	delete[] gPix;
	delete[] bPix;

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

	lutL->Dispose();
	lutA->Dispose();
	lutB->Dispose();

	delete lutL;
	delete lutA;
	delete lutB;

	currentCachedStage->Caches = currentCache;

	return currentCachedStage;
}

void LutImageOperation::Dispose()
{
}

void LutImageOperation::DisposeCacheEntry(ProcessingCacheEntry* cache)
{
	if (cache->Caches == nullptr)
		return;

	InternalImageData* currentCache = (InternalImageData*)cache->Caches;

	if (currentCache->RPixels != nullptr) {
		delete[] currentCache->RPixels;
	}
	if (currentCache->GPixels != nullptr) {
		delete[] currentCache->GPixels;
	}
	if (currentCache->BPixels != nullptr) {
		delete[] currentCache->BPixels;
	}

	delete currentCache;
}
