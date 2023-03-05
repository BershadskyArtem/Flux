#include "ImageDecoder.h"

AbstractImageDecoder::AbstractImageDecoder(std::string fileName) {
    this->m_fileName = fileName;
}

AbstractImageDecoder::~AbstractImageDecoder() {
    int i = 1;
}
