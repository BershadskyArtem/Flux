#pragma once
#include <turbojpeg.h>
#include <iostream>
#include "ImageEncoder.h"
#include "../../infrastructure/MemoryUtils.h"

class JpegImageEncoder : public AbstractImageEncoder{
protected:
	tjhandle m_JpegCompressor = nullptr;
public:
	JpegImageEncoder(pixel_t* pixels, int width, int height, int colorComponents = 3);
	JpegImageEncoder(byte_t* pixels, int width, int height, int colorComponents = 3);
	bool Init() override;
	bool FastSave(std::string filePath) override;
	~JpegImageEncoder();
};