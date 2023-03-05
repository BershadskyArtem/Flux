#pragma once
#include <string>
#include "../infrastructure/InteropTypes.h"

class AbstractImageDecoder {
protected:
    std::string m_fileName;
    bool m_HasErrors = false;
    int m_Width;
    int m_Height;
    int m_ScaledWidth;
    int m_ScaledHeight;

public:
    AbstractImageDecoder(std::string fileName);

    virtual bool Init() { return true; };
    virtual bool ReadGeneralMetadata(GeneralMetadata& data) = 0;
    virtual bool ReadExifMetadata(ExifMetadata& data) = 0;
    virtual bool ReadIptcMetadata(IptcMetadata& data) = 0;
    virtual bool ReadMakerMetadata(MakerMetadata& data) = 0;
    virtual bool HasErrors() = 0;
    virtual bool GetPreviewImage(uint8_t* buff, GeneralMetadata& data) = 0;
    virtual bool GetFullImage(float* buff) = 0;

    virtual int Width() = 0;
    virtual int Height() = 0;

    virtual int ScaledWidth() = 0;
    virtual int ScaledHeight() = 0;

    virtual ~AbstractImageDecoder();
};
