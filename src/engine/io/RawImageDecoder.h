#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <libraw/libraw.h>
#include "ImageDecoder.h"
#include "JpegImageDecoder.h"
#include "../infrastructure/InteropTypes.h"
#include "../infrastructure/BitmapHelper.h"

class RawImageDecoder : public AbstractImageDecoder {
private:
    LibRaw* processor;
    bool m_HasThumbnail;
    int m_ThumbnailIndex;
    libraw_processed_image_t* m_Image = nullptr;

    bool GetHalfSizedRaw(uint8_t* &buf, GeneralMetadata& data);
    bool GetPreviewFromJpeg(unsigned char* inMemoryJpeg,long inMemoryJpegSize, uint8_t* &buf, GeneralMetadata& data);
    bool GetPreviewFromBitmap(byte_t* bitmap, int width, int height,int channelsCount, uint8_t* &buf, GeneralMetadata& data);
    bool IsSupportedThumbnailformat(LibRaw_thumbnail_formats format);
    bool IsSupportedThumbnailformat(LibRaw_internal_thumbnail_formats format);

public:
    RawImageDecoder(std::string fileName);
    bool ReadGeneralMetadata(GeneralMetadata& data) override;
    bool ReadExifMetadata(ExifMetadata& data) override;
    bool ReadIptcMetadata(IptcMetadata& data) override;
    bool ReadMakerMetadata(MakerMetadata& data) override;

    bool Init() override;
    bool GetPreviewImage(uint8_t* &buf, GeneralMetadata& data) override;
    //bool LoadImage(float* buf) override;
    bool GetFullImage(float* buff) override;

    int ScaledWidth() override;
    int ScaledHeight() override;

    ~RawImageDecoder() override;
};