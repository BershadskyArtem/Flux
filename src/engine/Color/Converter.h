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
	static pixel_t sRGB2RGB(pixel_t in);
	static void sRGB2RGB(pixel_t* r, pixel_t* g , pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height);
	static void sRGB2RGB(pixel_t* rgbIn, pixel_t* rgbOut, int width, int height);
	static void sRGB2RGB(byte_t r, byte_t g , byte_t b, byte_t& rOut, byte_t& gOut, byte_t& bOut);

	static void RGB2OKLab(pixel_t* r, pixel_t* g, pixel_t* b, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height);
	static void RGB2OKLab(pixel_t rIn, pixel_t gIn, pixel_t bIn, pixel_t& lOut, pixel_t& aOut, pixel_t& bOut);

	static void OKLab2RGB(pixel_t lIn, pixel_t aIn, pixel_t bIn, pixel_t& rOut, pixel_t& gOut, pixel_t& bOut);
	static void OKLab2RGB(pixel_t* l, pixel_t* a, pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height);

	static void OkLab2OkLCh(pixel_t* l, pixel_t* a, pixel_t* b, pixel_t* lOut, pixel_t* cOut, pixel_t* hOut, int width, int height);
	static void OkLCh2OkLab(pixel_t* l, pixel_t* c, pixel_t* h, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height);

	static void OkLab2OkLCh(pixel_t l, pixel_t a, pixel_t b, pixel_t& lOut, pixel_t& cOut, pixel_t& hOut);
	static void OkLCh2OkLab(pixel_t l, pixel_t c, pixel_t h, pixel_t& lOut, pixel_t& aOut, pixel_t& bOut);


	static pixel_t RGB2sRGB(pixel_t in);

	static void Init();
	
private:
	static LUT cbrtLUT;
	static LUT LUTsRGBToRGBFloat;
	static LUT LUTRGBtosRGBFloat;
	static std::array<uint8_t, 256> RGB2sRGBLUT;
	static std::array<uint8_t, 256> sRGB2RGBLUT;
};

