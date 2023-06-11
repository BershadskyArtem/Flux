#include "Colors.h"


// RGB Color
sRGBColor RGBColor::TosRGB() {
	sRGBColor result = sRGBColor();

	result.R = Converter::RGB2sRGB(std::clamp(R, 0.f, 1.f));
	result.G = Converter::RGB2sRGB(std::clamp(G, 0.f, 1.f));
	result.B = Converter::RGB2sRGB(std::clamp(B, 0.f, 1.f));
	return result;
}
OkLabColor RGBColor::ToOkLab() {
	OkLabColor result = OkLabColor();
	Converter::RGB2OKLab(R, G, B, result.L, result.A, result.B);
	return result;
}
OkLChColor RGBColor::ToOkLCh() {
	OkLChColor result = OkLChColor();
	pixel_t tempL = 0.0f;
	pixel_t tempA = 0.0f;
	pixel_t tempB = 0.0f;
	Converter::RGB2OKLab(R, G, B, tempL, tempA, tempB);
	Converter::OkLab2OkLCh(tempL, tempA, tempB, result.L, result.C, result.H);
	return result;
}
u8ColorTriplet RGBColor::ToColorTriplet() {
	u8ColorTriplet result = u8ColorTriplet();
	result.R = static_cast<byte_t>(std::clamp(R, 0.0f, 1.f) * 255);
	result.G = static_cast<byte_t>(std::clamp(G, 0.0f, 1.f) * 255);
	result.B = static_cast<byte_t>(std::clamp(B, 0.0f, 1.f) * 255);
	return result;
}

//sRGB Color
RGBColor sRGBColor::ToRGB()
{
	RGBColor result = RGBColor();
	result.R = Converter::sRGB2RGB(std::clamp(R, 0.0f, 1.f));
	result.G = Converter::sRGB2RGB(std::clamp(G, 0.0f, 1.f));
	result.B = Converter::sRGB2RGB(std::clamp(B, 0.0f, 1.f));
	return result;
}
OkLabColor sRGBColor::ToOkLab()
{
	return ToRGB().ToOkLab();
}
OkLChColor sRGBColor::ToOkLCh()
{
	return ToRGB().ToOkLCh();
}
u8ColorTriplet sRGBColor::ToColorTriplet()
{
	u8ColorTriplet result = u8ColorTriplet();
	result.R = static_cast<byte_t>(std::clamp(R, 0.0f, 1.f) * 255);
	result.G = static_cast<byte_t>(std::clamp(G, 0.0f, 1.f) * 255);
	result.B = static_cast<byte_t>(std::clamp(B, 0.0f, 1.f) * 255);
	return result;
}

//OkLAB
sRGBColor OkLabColor::TosRGB() {
	return ToRGB().TosRGB();
}
RGBColor OkLabColor::ToRGB() {
	RGBColor result = RGBColor();
	Converter::OKLab2RGB(L, A, B, result.R, result.G, result.B);
	return result;
}
OkLChColor OkLabColor::ToOkLCh() {
	OkLChColor result = OkLChColor();
	Converter::OkLab2OkLCh(L, A, B, result.L, result.C, result.H);
	return result;
}

//OkLCh


sRGBColor OkLChColor::TosRGB() {
	return ToRGB().TosRGB();
}
RGBColor OkLChColor::ToRGB() {
	return ToOkLab().ToRGB();
}
OkLabColor OkLChColor::ToOkLab() {

	OkLabColor result = OkLabColor();
	Converter::OkLCh2OkLab(L, C, H, result.L, result.A, result.B);
	return result;
}

RGBColor::RGBColor()
{
	R = 0;
	G = 0;
	B = 0;
}

RGBColor::RGBColor(pixel_t r, pixel_t g, pixel_t b)
{
	R = r;
	G = g;
	B = b;
}


OkLChColor::OkLChColor()
{
	L = 0;
	C = 0;
	H = 0;
}

OkLChColor::OkLChColor(pixel_t r, pixel_t g, pixel_t b)
{
	L = r;
	C = g;
	H = b;
}