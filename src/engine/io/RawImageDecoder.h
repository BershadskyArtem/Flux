#pragma once
#include <algorithm>
#include <vector>
#include <libraw/libraw.h>
#include "ImageDecoder.h"
#include "../infrastructure/InteropTypes.h"


class RawImageDecoder : public AbstractImageDecoder {
private:
    LibRaw* processor;
    bool m_HasThumbnail;
    int m_ThumbnailIndex;
public:
    RawImageDecoder(std::string fileName);
    bool ReadGeneralMetadata(GeneralMetadata& data) override;
    bool ReadExifMetadata(ExifMetadata& data) override;
    bool ReadIptcMetadata(IptcMetadata& data) override;
    bool ReadMakerMetadata(MakerMetadata& data) override;

    bool Init() override;
    bool GetPreviewImage(uint8_t* buf, GeneralMetadata& data) override;
    //bool LoadImage(float* buf) override;
    bool GetFullImage(float* buff) override;

    int ScaledWidth() override;
    int ScaledHeight() override;

    ~RawImageDecoder() override;
};