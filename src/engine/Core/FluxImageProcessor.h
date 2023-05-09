#pragma once
#include <iostream>
#include <stack>
#include "../Color/ColorLUTConverter.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/PixelsHelper.h"
#include "../infrastructure/Matrix.h"
#include "Wavelets/Daubechies4.h"
#include "FluxWaveletDenoising.h"


class FluxImageProcessor {
private:
	static std::stack<BaseImageOperation*> s_Operations;

public:
	static void Init();
	static FluxImage* Process(FluxImage* image, ProcessSettings* settings);
	static ProcessingCache* PreProcess(FluxImage* image, ProcessSettings* settings);
	static FluxImage* FastProcess(FluxImage* image, ProcessSettings* settings);
	static ProcessingCache* ConstructCache(InternalImageData* image);
	static ProcessSettings* DefaultSettings();
	static FluxImage* FastProcessToBitmap(ProcessingCache* cache, ProcessSettings* settings);
	static void DisposeLayer(ProcessingLayerCache* layer);
	static void DisposeStageCache(ProcessingCacheEntry* entry);
	//static FluxImage* ProcessToFile(FluxImage* image, ProcessSettings* settings);
};