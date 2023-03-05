#pragma once
#include <algorithm>
#include <cmath>
#include <string>
#include <fstream>
#include "FLXDefines.h"


class MemoryUtils {
public:
    static void Copy(const char* buf, char* out, int bytesPerItem, int numberOfItems);
    ///
    /// \param buf Input buffer
    /// \param out Output buffer
    /// \param bytesPerItem Number of channels
    /// \param numberOfItems Pixels count
    static void CopyFromUint8ToFloat(unsigned char* buf, float* out, int channels, int numberOfItems, bool normalize = true);
    static void CopyFromFloatToUint8(float* buf, uint8_t* out, int channels, int numberOfItems, bool normalize = true);
    static bool SaveBinaryToDisk(byte_t* data, unsigned long size, std::string fileName);
};