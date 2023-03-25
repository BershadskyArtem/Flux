#include "ImageInput.h"


ImageInput::ImageInput(std::string fileName) {
	//this->decoder = new JpegImageDecoder(fileName);

	std::string lower = fileName;

	for (auto& c : lower)
	{
		c = std::tolower(c);
	}


	if (lower.ends_with(".jpeg") || lower.ends_with(".jpg")) {
		this->decoder = new JpegImageDecoder(fileName);
	}
	else if (lower.ends_with(".cr2") ||
		lower.ends_with(".cr3") ||
		lower.ends_with(".crw") ||
		lower.ends_with(".nef") ||
		lower.ends_with(".raf") ||
		lower.ends_with(".dng") ||
		lower.ends_with(".mos") ||
		lower.ends_with(".kdc") ||
		lower.ends_with(".arw") ||
		lower.ends_with(".dcr"))
	{
		this->decoder = new RawImageDecoder(fileName);
	}
}

GeneralMetadata* ImageInput::ReadGeneralMetadata() {
	GeneralMetadata* data = new GeneralMetadata;
	bool success = decoder->ReadGeneralMetadata(*data);

	if (!success) {
		delete data;
		m_HasErrors = true;
	}

	return success ? data : nullptr;
}

GeneralMetadata* ImageInput::ReadPreviewGeneralMetadata()
{
	GeneralMetadata* data = new GeneralMetadata();
	bool success = decoder->ReadPreviewGeneralMetadata(*data);
	if (!success) {
		delete data;
		m_HasErrors = true;
	}
	return success ? data : nullptr;
}

bool ImageInput::HasErrors() {
	return decoder->HasErrors() || m_HasErrors;
}

ImageInput::~ImageInput() {
	delete decoder;
}

MakerMetadata* ImageInput::ReadMakerMetadata() {
	return nullptr;
}

IptcMetadata* ImageInput::ReadIptcMetadata() {
	return nullptr;
}

ExifMetadata* ImageInput::ReadExifMetadata() {
	ExifMetadata* data = new ExifMetadata();

	bool result = decoder->ReadExifMetadata((*data));
	if (!result) {
		//TODO: Potential memory leaks in data.
		delete data;
	}
	return result ? data : nullptr;
}

FluxImage* ImageInput::DecodeImage() {
	float* buf = new float[Width() * Height() * 3];

	bool result = decoder->GetFullImage(buf);

	if (!result) {
		delete[] buf;
		m_HasErrors = true;
		return nullptr;
	}

	FluxImage* image = new FluxImage();

	image->Pixels = buf;
	image->Width = Width();
	image->Height = Height();
	image->Type = PixelType::Float;

	return image;
}

FluxImage* ImageInput::GetPreview() {

	int h = this->ScaledHeight();
	int w = this->ScaledWidth();

	byte_t* pixels = new byte_t[w * h * 3];

	bool success = decoder->GetPreviewImage(pixels);

	if (!success) {
		delete[] pixels;
		return nullptr;
	}
		
	FluxImage* image = new FluxImage();
	image->Width = w;
	image->Height = h;
	image->Type = PixelType::UnsignedByte;
	image->Pixels = pixels;

	return image;
}

bool ImageInput::Init() {
	return decoder->Init();
}

int ImageInput::Width() {
	if (decoder == nullptr)
		return 0;

	if (decoder->HasErrors())
		return 0;

	return decoder->Width();
}

int ImageInput::Height() {
	if (decoder == nullptr)
		return 0;

	if (decoder->HasErrors())
		return 0;

	return decoder->Height();
}

int ImageInput::ScaledWidth()
{
	if (decoder == nullptr)
		return 0;
	if (decoder->HasErrors())
		return 0;

	return decoder->ScaledWidth();
}

int ImageInput::ScaledHeight()
{
	if (decoder == nullptr)
		return 0;
	if (decoder->HasErrors())
		return 0;
	return decoder->ScaledHeight();
}

void ImageInput::FreeFluxImage(FluxImage* image) {
	try {
		if (image->Pixels != nullptr)
			delete[] image->Pixels;
		image->Pixels = nullptr;
		delete image;
	}
	catch (std::exception n) {

	}

}