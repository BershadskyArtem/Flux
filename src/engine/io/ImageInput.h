#pragma once
#include <string>
#include "ImageDecoder.h"
#include "../infrastructure/InteropTypes.h"
#include "JpegImageDecoder.h"
#include "RawImageDecoder.h"

class ImageInput {
protected:
    std::string filePath;
    AbstractImageDecoder* decoder = nullptr;
    bool m_HasErrors;
public:
    ImageInput(std::string fileName);
    FluxImage* GetPreview();
    FluxImage* DecodeImage();
    GeneralMetadata* ReadGeneralMetadata();
    GeneralMetadata* ReadPreviewGeneralMetadata();
    ExifMetadata* ReadExifMetadata();
    IptcMetadata* ReadIptcMetadata();
    MakerMetadata* ReadMakerMetadata();

    static void FreeFluxImage(FluxImage* image);

    int Width();
    int Height();

    int ScaledWidth();
    int ScaledHeight();

    bool Init();
    bool HasErrors();
    ~ImageInput();
};