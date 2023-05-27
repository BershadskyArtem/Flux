#include "LutImageOperation.h"

pixel_t LutImageOperation::GetBrightnessUsingColorMask(pixel_t l, pixel_t a, pixel_t b, HSLColorProcessingSettings& settings)
{
	//pixel_t result = 0.0f;
	//settings.


		//return pixel_t();
	return 0.0f;
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
	//settings.HSLSettings.
	//HSL
	//pixel_t v = GetBrightnessUsingColorMask(pixel_t l, pixel_t a, pixel_t b, settings);

}

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

				rF = Converter::RGB2sRGB(rF);
				gF = Converter::RGB2sRGB(gF);
				bF = Converter::RGB2sRGB(bF);

				lutL->SetValue(rF, r, g, b);
				lutA->SetValue(gF, r, g, b);
				lutB->SetValue(bF, r, g, b);
			}
		}
	}

	std::string outputFilePath1 = "C:\\Users\\Artyom\\Downloads\\Lena-B-End.jpg";
	JpegImageEncoder encoder2 = JpegImageEncoder((pixel_t*)previousCache->BPixels, w, h, 1);
	encoder2.Init();
	encoder2.FastSave(outputFilePath1);

	pixel_t* rPix = new pixel_t[w * h]{};
	pixel_t* gPix = new pixel_t[w * h]{};
	pixel_t* bPix = new pixel_t[w * h]{};

	Converter::OKLab2RGB(previousCache->LPixels, previousCache->APixels, previousCache->BPixels, rPix, gPix, bPix, w, h);
	
	std::string outputFilePath2 = "C:\\Users\\Artyom\\Downloads\\Lena-R.jpg";
	JpegImageEncoder encoder1 = JpegImageEncoder((pixel_t*)rPix, w, h, 1);	
	encoder1.Init();
	encoder1.FastSave(outputFilePath2);



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
