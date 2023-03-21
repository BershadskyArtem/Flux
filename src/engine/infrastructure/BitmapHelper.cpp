#include "BitmapHelper.h"

int BitmapHelper::GetStride(int width, int height, int bitsPerPixel)
{
	int bpp = bitsPerPixel;
	int stride = width * bpp;  // bits per row
	stride += 31;            // round up to next 32-bit boundary
	stride /= 32;            // DWORDs per row
	stride *= 4;
	return stride;
}

int BitmapHelper::GetStrideDifference(int width, int height, int bitsPerPixel)
{
	int bpp = bitsPerPixel;
	int stride = width * bpp;  // bits per row
	stride += 31;            // round up to next 32-bit boundary
	stride /= 32;            // DWORDs per row
	stride *= 4;
	int strideDelta = abs(width * 3 - stride);
	return strideDelta;
}
