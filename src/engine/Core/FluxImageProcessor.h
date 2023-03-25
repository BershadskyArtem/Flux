#pragma once
#include "../Color/ColorLUTConverter.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/PixelsHelper.h"
#include <iostream>

class FluxImageProcessor {
public:
	static FluxImage* Process(FluxImage* image, ProcessSettings* settings);
};