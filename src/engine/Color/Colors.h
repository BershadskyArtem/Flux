#pragma once

#include <algorithm>
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/FluxMath.h"
#include "Converter.h"


struct RGBColor;
struct sRGBColor;
struct OkLabColor;
struct OkLChColor;
struct u8ColorTriplet;

struct RGBColor{
public:
	pixel_t R;
	pixel_t G;
	pixel_t B;

	RGBColor();
	RGBColor(pixel_t r, pixel_t g, pixel_t b);


	sRGBColor TosRGB();
	OkLabColor ToOkLab();
	OkLChColor ToOkLCh();
	u8ColorTriplet ToColorTriplet();
};

struct sRGBColor {
public:
	pixel_t R;
	pixel_t G;
	pixel_t B;

	RGBColor ToRGB();
	OkLabColor ToOkLab();
	OkLChColor ToOkLCh();
	u8ColorTriplet ToColorTriplet();
};

struct OkLabColor {
public:
	pixel_t L;
	pixel_t A;
	pixel_t B;

	sRGBColor TosRGB();
	RGBColor ToRGB();
	OkLChColor ToOkLCh();
};


struct OkLChColor {
	pixel_t L;
	pixel_t C;
	pixel_t H;

	OkLChColor();
	OkLChColor(pixel_t l, pixel_t c, pixel_t h);

	sRGBColor TosRGB();
	RGBColor ToRGB();
	OkLabColor ToOkLab();
};

struct u8ColorTriplet {
	byte_t R;
	byte_t G;
	byte_t B;
};