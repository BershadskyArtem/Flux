#include "WaveletImage.h"

WaveletLine::WaveletLine(pixel_t* hi, pixel_t* lo, int length) {
	Hi = hi;
	Lo = lo;
	Length = length;
}

WaveletData::~WaveletData()
{
	Dispose();
}
