#include "ImageDecoder.h"

AbstractImageDecoder::AbstractImageDecoder(std::string fileName) {
    this->m_fileName = fileName;
}

AbstractImageDecoder::~AbstractImageDecoder() {
    int i = 1;
}

int AbstractImageDecoder::Width()
{
    return m_Width;
}

int AbstractImageDecoder::Height()
{
    return m_Height;
}
