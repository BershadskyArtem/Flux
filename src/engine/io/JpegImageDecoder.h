#pragma once

#include <string>
#include <turbojpeg.h>
#include <fstream>
#include "ImageDecoder.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/MemoryUtils.h"
#include "../Color/Converter.h"

class JpegImageDecoder : public AbstractImageDecoder {
private:
    const tjscalingfactor DEFAULT_SCALING = { 1,1 };
    const tjscalingfactor THUMBNAIL_SCALING = { 1,2 };
    tjhandle m_JpegHandle = nullptr;
    unsigned long m_JpegSize = -1;
    unsigned char* m_JpegFile = nullptr;
    unsigned char* m_InMemoryJpeg = nullptr;
    long m_BufSize = -1;
    bool m_IsInitSuccessful = true;
    bool m_IsFromMemory = false;
public:
    JpegImageDecoder(std::string fileName);
    JpegImageDecoder(unsigned char* inMemoryJpeg, long bufSize);
    bool ReadGeneralMetadata(GeneralMetadata& data) override;
    bool ReadPreviewGeneralMetadata(GeneralMetadata& data) override;
    bool ReadExifMetadata(ExifMetadata& data) override;
    bool ReadIptcMetadata(IptcMetadata& data) override;
    bool ReadMakerMetadata(MakerMetadata& data) override;
    bool Init() override;
    bool HasErrors() override;
    bool GetPreviewImage(uint8_t* buf) override;
    bool GetFullImage(float* buff) override;

    int ScaledWidth() override;
    int ScaledHeight() override;

    ~JpegImageDecoder() override;
};