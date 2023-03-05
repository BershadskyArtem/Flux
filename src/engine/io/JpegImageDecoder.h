#pragma once

#include <string>
#include <turbojpeg.h>
#include <fstream>
#include "ImageDecoder.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/MemoryUtils.h"

class JpegImageDecoder : public AbstractImageDecoder {
private:
    const tjscalingfactor DEFAULT_SCALING = { 1,1 };
    const tjscalingfactor THUMBNAIL_SCALING = { 1,2 };
    tjhandle m_JpegHandle = nullptr;
    unsigned long m_JpegSize = -1;
    unsigned char* m_JpegFile = nullptr;
    bool m_IsInitSuccessful = true;
public:
    JpegImageDecoder(std::string fileName);
    bool ReadGeneralMetadata(GeneralMetadata& data) override;
    bool ReadExifMetadata(ExifMetadata& data) override;
    bool ReadIptcMetadata(IptcMetadata& data) override;
    bool ReadMakerMetadata(MakerMetadata& data) override;

    bool Init() override;
    bool HasErrors() override;
    bool GetPreviewImage(uint8_t* buf, GeneralMetadata& data) override;
    //bool LoadImage(float* buf) override;
    bool GetFullImage(float* buff) override;

    int Width() override;
    int Height() override;

    int ScaledWidth() override;
    int ScaledHeight() override;

    ~JpegImageDecoder() override;
};