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

template<typename T>
WaveletImage<T>::WaveletImage()
{

}

template struct WaveletImage<pixel_t>;