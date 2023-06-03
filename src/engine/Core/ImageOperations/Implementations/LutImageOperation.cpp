#include "LutImageOperation.h"
#include "LutImageOperation.h"
#include "LutImageOperation.h"
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
pixel_t LutImageOperation::ElevateBrightness(pixel_t l, pixel_t a, pixel_t b, LUTProcessingSettings& settings)
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
	
	//Gamma = brightness
	// 
	//currentLuma = std::pow(currentLuma, brightness);
	// 
	//Then contrast

	pixel_t contrast = settings.LightSettings.Contrast / 100.f;
	pixel_t contrastCurveAtPoint = FluxMath::ContrastSmoothstep(currentLuma);
	currentLuma = FluxMath::LinearInterpolate(contrast, contrastCurveAtPoint, currentLuma);

	pixel_t totalElevation = 1.f;

	pixel_t whFactor = FluxMath::DualSmoothstep(luma, 1.f, 0.9f, -0.85f);
	pixel_t hiFactor = FluxMath::DualSmoothstep(luma, 1.f, 0.9f, -0.75f);
	pixel_t blFactor = FluxMath::DualSmoothstep(luma, 0.f, 0.4f, -0.37f);
	pixel_t shFactor = FluxMath::DualSmoothstep(luma, 0.f, 0.5f, -0.45f);

	totalElevation -= whFactor * settings.HDRSettings.Whites / 100.f +
		blFactor * settings.HDRSettings.Blacks / 100.f +
		hiFactor * settings.HDRSettings.Highlights / 100.f +
		shFactor * settings.HDRSettings.Shadows / 100.f;

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

	totalElevation -= brightness + colorFactorr * settings.HSLSettings.Red.Lightness / 100.f +
		colorFactoro * settings.HSLSettings.Orange.Lightness / 100.f +
		colorFactory * settings.HSLSettings.Yellow.Lightness / 100.f +
		colorFactorg * settings.HSLSettings.Green.Lightness / 100.f +
		colorFactora * settings.HSLSettings.Aqua.Lightness / 100.f +
		colorFactorb * settings.HSLSettings.Blue.Lightness / 100.f +
		colorFactorm * settings.HSLSettings.Magenta.Lightness / 100.f +
		colorFactorp * settings.HSLSettings.Purple.Lightness / 100.f;

	for (int i = 0; i < settings.HSLSettings.CustomColorsCount; i++)
	{
		totalElevation -= GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.CustomColors[i]) 
			* settings.HSLSettings.CustomColors[i].Lightness / 100.f;
	}

	totalElevation = std::max(totalElevation, 0.0001f);
	totalElevation = std::min(totalElevation, 1000.f);
	
	currentLuma = std::pow(currentLuma, totalElevation);
	
	return currentLuma;
}

pixel_t LutImageOperation::ElevateHue(pixel_t l, pixel_t a, pixel_t b, LUTProcessingSettings& settings)
{
	pixel_t elevateAmount = 0.f;
	pixel_t chrominance = 0.f;
	pixel_t hue = 0.f;
	pixel_t luma = 0.f;
	pixel_t currentHue = 0.f;

	Converter::OkLab2OkLCh(l, a, b, luma, chrominance, hue);


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
		colorFactoro * settings.HSLSettings.Orange.Lightness / (2 * PI) +
		colorFactory * settings.HSLSettings.Yellow.Lightness / (2 * PI) +
		colorFactorg * settings.HSLSettings.Green.Lightness / (2 * PI) +
		colorFactora * settings.HSLSettings.Aqua.Lightness / (2 * PI) +
		colorFactorb * settings.HSLSettings.Blue.Lightness / (2 * PI) +
		colorFactorm * settings.HSLSettings.Magenta.Lightness / (2 * PI) +
		colorFactorp * settings.HSLSettings.Purple.Lightness / (2 * PI);

	for (int i = 0; i < settings.HSLSettings.CustomColorsCount; i++)
	{
		totalElevation += GetBrightnessUsingColorMask(luma, chrominance, hue, settings.HSLSettings.CustomColors[i])
			* settings.HSLSettings.CustomColors[i].HueShift / (2 * PI);
	}

	currentHue = hue + totalElevation;
	return currentHue;

}

pixel_t LutImageOperation::ElevateSaturation(pixel_t l, pixel_t a, pixel_t b, LUTProcessingSettings& settings)
{

	pixel_t sF = 0;
	pixel_t lF = 0;
	pixel_t hF = 0;



	return pixel_t();
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
				pixel_t rF = r / LUT_DIM;
				pixel_t gF = g / LUT_DIM;
				pixel_t bF = b / LUT_DIM;


				pixel_t lPix = 0;
				pixel_t aPix = 0;
				pixel_t bPix = 0;

				pixel_t lPixFinal = 0;
				pixel_t hPixFinal = 0;
				pixel_t sPixFinal = 0;

				Converter::RGB2OKLab(rF, gF, bF, lPix, aPix, bPix);

				lPixFinal = ElevateBrightness(lPix, aPix, bPix, settings);
				hPixFinal = ElevateHue(lPix, aPix, bPix, settings);
				sPixFinal = ElevateSaturation(lPix, aPix, bPix, settings);


				Converter::OKLab2RGB(lPixFinal, hPixFinal, bPix, rF, gF, bF);

				rF = std::max(std::min(rF, 1.f), 0.f);
				gF = std::max(std::min(gF, 1.f), 0.f);
				bF = std::max(std::min(bF, 1.f), 0.f);

				rF = Converter::RGB2sRGB(rF);
				gF = Converter::RGB2sRGB(gF);
				bF = Converter::RGB2sRGB(bF);

				lutL->SetValue(rF, r, g, b);
				lutA->SetValue(gF, r, g, b);
				lutB->SetValue(bF, r, g, b);
			}
		}
	}

	//std::string outputFilePath1 = "C:\\Users\\Artyom\\Downloads\\Lena-B-End.jpg";
	//JpegImageEncoder encoder2 = JpegImageEncoder((pixel_t*)previousCache->BPixels, w, h, 1);
	//encoder2.Init();
	//encoder2.FastSave(outputFilePath1);

	pixel_t* rPix = new pixel_t[w * h]{};
	pixel_t* gPix = new pixel_t[w * h]{};
	pixel_t* bPix = new pixel_t[w * h]{};

	//auto oklab2rgbtime = BenchmarkHelper::StartWatch();

	Converter::OKLab2RGB(previousCache->LPixels, previousCache->APixels, previousCache->BPixels, rPix, gPix, bPix, w, h);

	//BenchmarkHelper::ShowDurationFinal(oklab2rgbtime, "OkLAB to RGB took...");

	//std::string outputFilePath2 = "C:\\Users\\Artyom\\Downloads\\Lena-R.jpg";
	//JpegImageEncoder encoder1 = JpegImageEncoder((pixel_t*)rPix, w, h, 1);	
	//encoder1.Init();
	//encoder1.FastSave(outputFilePath2);
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
