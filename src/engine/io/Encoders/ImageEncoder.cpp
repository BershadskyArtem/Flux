#include "ImageEncoder.h"

AbstractImageEncoder::AbstractImageEncoder(pixel_t* pixels, int width, int height, int colorComponents)
{
	m_Width = width;
	m_Height = height;
	m_pixels = (void*)pixels;
	m_ColorComponents = colorComponents;
	m_PixelType = PixelType::Float;


	if (colorComponents < 1 || colorComponents > 4) {
		m_HasErrors = true;
	}
}

AbstractImageEncoder::AbstractImageEncoder(byte_t* pixels, int width, int height, int colorComponents)
{
	m_Width = width;
	m_Height = height;
	m_pixels = (void*)pixels;
	m_ColorComponents = colorComponents;
	m_PixelType = PixelType::UnsignedByte;

	if (colorComponents < 1 || colorComponents > 4) {
		m_HasErrors = true;
	}

}

bool AbstractImageEncoder::HasErrors()
{
	return m_HasErrors;
}

AbstractImageEncoder::~AbstractImageEncoder()
{
}
