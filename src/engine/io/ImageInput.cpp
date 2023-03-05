#include "ImageInput.h"


ImageInput::ImageInput(std::string fileName) {
    //this->decoder = new JpegImageDecoder(fileName);
    this->decoder = new JpegImageDecoder(fileName);
}

GeneralMetadata* ImageInput::ReadGeneralMetadata() {
    GeneralMetadata* metadata = new GeneralMetadata;
    m_HasErrors = !decoder->ReadGeneralMetadata(*metadata);
    return metadata;
}

bool ImageInput::HasErrors() {
    return decoder->HasErrors() || m_HasErrors;
}

ImageInput::~ImageInput() {

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
        delete buf;
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

    int h = this->decoder->ScaledHeight();
    int w = this->decoder->ScaledWidth();

    uint8_t* pixels = new uint8_t[w * h * 3];

    GeneralMetadata metadata;

    m_HasErrors = !decoder->GetPreviewImage(pixels, metadata);

    if (m_HasErrors)
        return nullptr;


    FluxImage* image = new FluxImage();
    image->Width = metadata.Width;
    image->Height = metadata.Height;
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