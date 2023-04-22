//
// Created by Artyom on 08.12.2022.
//


#include "MemoryUtils.h"

void MemoryUtils::Copy(const char* buf, char* out, int bytesPerItem, int numberOfItems) {
    int bufferSize = bytesPerItem * numberOfItems;
    std::copy(buf, buf + bufferSize, out);
}

void MemoryUtils::CopyFromUint8ToFloat(unsigned char* buf, float* out, int channels, int numberOfItems, bool normalize) {
    int uintSize = 1 * channels * numberOfItems;

    if (normalize) {
#pragma omp parallel for
        for (int i = 0; i < uintSize; ++i) {
            out[i] = std::clamp(buf[i] / 255.f, 0.f, 1.f);
        }
    }
    else {
#pragma omp parallel for
        for (int i = 0; i < uintSize; ++i) {
            out[i] = static_cast<float>(buf[i]);
        }
    }

}

void MemoryUtils::CopyFromUint16ToFloat(uint16_t* buf, float* out, int channels, int numberOfItems, bool normalize)
{
    int uintSize = 1 * channels * numberOfItems;

    if (normalize) {
//#pragma omp parallel for
        for (int i = 0; i < uintSize; ++i) {
            float d = buf[i] / 65536.f;
            out[i] = std::clamp(d, 0.00001f, 1.f);
        }
    }
    else {
//#pragma omp parallel for
        for (int i = 0; i < uintSize; ++i) {
            out[i] = static_cast<float>(buf[i]);
        }
    }
}

void MemoryUtils::CopyFromFloatToUint8(float* buf, uint8_t* out, int channels, int numberOfItems, bool normalize)
{
    int size = channels * numberOfItems;
    if (normalize) {
#pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            out[i] = static_cast<uint8_t>(std::clamp(buf[i] * 255.f, 0.f, 255.f));
        }
    }
    else {
#pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            out[i] = static_cast<uint8_t>(std::clamp(buf[i], 0.f, 255.f));
        }
    }
}

bool MemoryUtils::SaveBinaryToDisk(byte_t* data, unsigned long size, std::string fileName)
{
    std::ofstream outputStream(fileName, std::ios::out | std::ios::binary | std::ios::app);
    if (!outputStream.is_open()) {
        outputStream.close();
        return false;
    }
    outputStream.write((char*)data, size);
    outputStream.close();
    return true;
}

void MemoryUtils::TransposeMatrix(std::vector<pixel_t*> matrix, int width, int height)
{




}
