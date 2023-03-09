#include "JpegImageEncoder.h"

JpegImageEncoder::JpegImageEncoder(pixel_t* pixels, int width, int height, int colorComponents) :AbstractImageEncoder(pixels, width, height, colorComponents)
{
}

JpegImageEncoder::JpegImageEncoder(byte_t* pixels, int width, int height, int colorComponents) :AbstractImageEncoder(pixels, width, height, colorComponents)
{
}

bool JpegImageEncoder::Init()
{
	m_JpegCompressor = tjInitCompress();
	if (m_JpegCompressor == nullptr) {
		m_HasErrors = true;
	}
	return HasErrors();
}

bool JpegImageEncoder::FastSave(std::string filePath)
{
	int jpegQuality = 80;
	
	int pixel_type = 0;
	int sampling = 0;

	if (m_ColorComponents == 4) {
		pixel_type = TJPF_CMYK;
	}
		

	if (m_ColorComponents == 3) {
		sampling = TJSAMP_422;
		pixel_type = TJPF_RGB;
	}

	if (m_ColorComponents == 1) {
		sampling = TJSAMP_GRAY;
		pixel_type = TJPF_GRAY;
	}

	byte_t* pixels = new byte_t[m_Width * m_Height * m_ColorComponents];

	if (m_PixelType == PixelType::Float) {
		MemoryUtils::CopyFromFloatToUint8((float*)m_pixels, pixels, m_ColorComponents, m_Width * m_Height);
	}
	else if (m_PixelType == PixelType::UnsignedByte) {
		MemoryUtils::Copy((char*)m_pixels, (char*)pixels, 1, m_Width * m_Height * m_ColorComponents);
	}

	byte_t* jpegBuffer = nullptr;
	unsigned long jpegSize = 0;

	int result = tjCompress2(m_JpegCompressor, (byte_t*)pixels, m_Width, 0, m_Height, pixel_type, &jpegBuffer, &jpegSize, sampling, jpegQuality, TJFLAG_ACCURATEDCT);

	if (result != 0) {

		std::string v = tjGetErrorStr2(m_JpegCompressor);

		if (jpegBuffer != nullptr)
			tjFree(jpegBuffer);

		delete[] pixels;
		jpegBuffer = nullptr;
		m_HasErrors = true;
		return !HasErrors();
	}

	if (!MemoryUtils::SaveBinaryToDisk(jpegBuffer, jpegSize, filePath)) {
		m_HasErrors = true;
	}

	tjFree(jpegBuffer);
	delete[] pixels;
	return !HasErrors();
}

JpegImageEncoder::~JpegImageEncoder()
{
	tjDestroy(m_JpegCompressor);
}
