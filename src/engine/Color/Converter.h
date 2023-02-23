#pragma once
#include <algorithm>
#include <execution>
#include <array>
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/LUT.h"

class Converter
{
public:
	//sRGB
	static void sRGB2RGB(pixel_t r, pixel_t g , pixel_t b, pixel_t& rOut, pixel_t& gOut, pixel_t& bOut);
	static void sRGB2RGB(pixel_t* r, pixel_t* g , pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height);
	static void sRGB2RGB(byte_t r, byte_t g , byte_t b, byte_t& rOut, byte_t& gOut, byte_t& bOut);
	static void Init();
	
private:
	static LUT cbrtLUT;
	static LUT LUTsRGBToRGBFloat;
	static LUT LUTRGBtosRGBFloat;
	static std::array<uint8_t, 256> RGB2sRGBLUT;
	static std::array<uint8_t, 256> sRGB2RGBLUT;
};

