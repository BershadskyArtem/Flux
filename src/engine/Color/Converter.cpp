#include "Converter.h"



std::array<uint8_t, 256> Converter::RGB2sRGBLUT = {
0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,11,11,11,12,12,13,13,13,14,14,15,15,16,16,16,17,17,18,18,19,19,20,20,21,22,22,23,23,24,24,25,26,26,27,27,28,29,29,30,31,31,32,33,33,34,35,36,36,37,38,38,39,40,41,42,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,64,65,66,67,68,70,71,72,73,74,75,76,77,78,80,81,82,83,84,85,87,88,89,90,92,93,94,95,97,98,99,101,102,103,105,106,107,109,110,112,113,114,116,117,119,120,122,123,125,126,128,129,131,132,134,135,137,139,140,142,144,145,147,148,150,152,153,155,157,159,160,162,164,166,167,169,171,173,175,176,178,180,182,184,186,188,190,192,193,195,197,199,201,203,205,207,209,211,213,215,218,220,222,224,226,228,230,232,235,237,239,241,243,245,248,250,252,255
};

void Converter::sRGB2RGB(pixel_t r, pixel_t g, pixel_t b, pixel_t& rOut, pixel_t& gOut, pixel_t& bOut)
{
	rOut = Converter::LUTsRGBToRGBFloat.Get01(r);
	gOut = Converter::LUTsRGBToRGBFloat.Get01(g);
	bOut = Converter::LUTsRGBToRGBFloat.Get01(b);
}

void Converter::sRGB2RGB(pixel_t* r, pixel_t* g, pixel_t* b, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, int width, int height)
{

	int size = width * height;
	int inc = vfloat::size;

	vfloat zeroV = 0.f;
	vfloat oneV = 1.f;

#pragma omp parallel for
	for (int i = 0; i < size; i += inc)
	{
		auto rV = vfloat::load_aligned(&r[i]);
		auto gV = vfloat::load_aligned(&g[i]);
		auto bV = vfloat::load_aligned(&b[i]);

		rV = xsimd::clip(rV, zeroV, oneV);
		gV = xsimd::clip(gV, zeroV, oneV);
		bV = xsimd::clip(bV, zeroV, oneV);

		rV = Converter::LUTsRGBToRGBFloat.Get01(rV);
		gV = Converter::LUTsRGBToRGBFloat.Get01(gV);
		bV = Converter::LUTsRGBToRGBFloat.Get01(bV);

		rV.store_aligned(&rOut[i]);
		gV.store_aligned(&gOut[i]);
		bV.store_aligned(&bOut[i]);
	}
}

void Converter::sRGB2RGB(byte_t r, byte_t g, byte_t b, byte_t& rOut, byte_t& gOut, byte_t& bOut)
{
	rOut = RGB2sRGBLUT[r];
	gOut = RGB2sRGBLUT[g];
	bOut = RGB2sRGBLUT[b];
}

void Converter::Init()
{
	Converter::cbrtLUT.Init(65535);
	Converter::LUTsRGBToRGBFloat.Init(65535);
	Converter::LUTRGBtosRGBFloat.Init(65535);

	for (int i = 0; i < 65535; i++)
	{
		float v = cbrtf(i / 65535.f);
		Converter::cbrtLUT.SetValue(v, i);
	}

	for (int i = 0; i < 65535; i++)
	{
		float v = i / 65535.f;
		if (v <= 0.0031308) {
			Converter::LUTRGBtosRGBFloat.SetValue(v * 12.92f, i);
		}
		else {
			Converter::LUTRGBtosRGBFloat.SetValue(1.055 * std::pow(v, 1.0 / 2.4) - 0.055, i);
		}
	}
	for (int i = 0; i < 65535; i++)
	{
		float v = i / 65535.f;
		float vR = 0.f;
		if (v <= 0.04045f) {
			vR = v / 12.92f;
		}
		else {
			vR = std::powf((v + 0.055f) / 1.055f, 2.4f);
		}
		Converter::LUTsRGBToRGBFloat.SetValue(vR, i);
	}
}
