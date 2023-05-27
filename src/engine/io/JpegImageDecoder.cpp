#include "JpegImageDecoder.h"

JpegImageDecoder::JpegImageDecoder(unsigned char* inMemoryJpeg, long bufSize) : AbstractImageDecoder("")
{
    m_IsFromMemory = true;
    m_InMemoryJpeg = inMemoryJpeg;
    m_BufSize = bufSize;
}

JpegImageDecoder::JpegImageDecoder(std::string fileName) : AbstractImageDecoder(fileName) {
    m_IsFromMemory = false;
}


bool JpegImageDecoder::ReadMakerMetadata(MakerMetadata& data) {
    return false;
}

bool JpegImageDecoder::ReadIptcMetadata(IptcMetadata& data) {
    return false;
}

bool JpegImageDecoder::ReadExifMetadata(ExifMetadata& data) {

    return false;
}

bool JpegImageDecoder::ReadGeneralMetadata(GeneralMetadata& data) {
    if (HasErrors()) {
        return !m_HasErrors;
    }
    int chrominance = 0;
    int subsampling = 0;
    int width = 0;
    int height = 0;
    int result = tjDecompressHeader3(m_JpegHandle, m_JpegFile, m_JpegSize, &width, &height, &chrominance,
        &subsampling);

    data.Width = width;
    data.Height = height;

    bool hadErrors = result != 0;
    m_HasErrors = m_HasErrors || hadErrors;
    return !hadErrors;
}

bool JpegImageDecoder::ReadPreviewGeneralMetadata(GeneralMetadata& data)
{
    data.Width = m_ScaledWidth;
    data.Height = m_ScaledHeight;
    return !HasErrors();
}

bool JpegImageDecoder::HasErrors() {
    return m_HasErrors;
}

bool JpegImageDecoder::Init() {
    if (m_IsFromMemory) {
        m_JpegFile = (unsigned char*)m_InMemoryJpeg;
        m_JpegSize = m_BufSize;
    }
    else {
        std::ifstream file(m_fileName.c_str(), std::ios::binary | std::ios::ate);

        if (!file) {
            m_HasErrors = true;
            return !m_HasErrors;
        }

        std::streamsize size = file.tellg();

        if (size <= 0) {
            m_HasErrors = true;
            return !m_HasErrors;
        }

        m_JpegSize = static_cast<unsigned long>(size);
        m_JpegFile = tjAlloc(m_JpegSize);

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(m_JpegFile), m_JpegSize);
        file.close();
    }

    m_JpegHandle = tjInitDecompress();
    if (!m_JpegHandle) {
        m_HasErrors = true;
        return !m_HasErrors;
    }

    GeneralMetadata metadata;
    metadata.Height = -1;
    metadata.Width = -1;

    if (!this->ReadGeneralMetadata(metadata)) {
        m_HasErrors = true;
        return !HasErrors();
    }

    m_Width = metadata.Width;
    m_Height = metadata.Height;

    m_ScaledWidth = TJSCALED(m_Width, THUMBNAIL_SCALING);
    m_ScaledHeight = TJSCALED(m_Height, THUMBNAIL_SCALING);

    return !HasErrors();
}


JpegImageDecoder::~JpegImageDecoder() {
    if (m_JpegHandle != nullptr) {
        tjDestroy(m_JpegHandle);
        m_JpegHandle = nullptr;
    }
    //If we own this memory
    if (m_JpegFile != nullptr && !m_IsFromMemory) {
        tjFree(m_JpegFile);
        m_JpegFile = nullptr;
    }
}

bool JpegImageDecoder::GetPreviewImage(uint8_t* buf) {
    int pixelFormat = TJPF_RGB;

    auto pixelsBuf = tjAlloc(m_ScaledWidth * m_ScaledHeight * tjPixelSize[pixelFormat]);

    if (pixelsBuf == nullptr) {
        m_HasErrors = true;
        return !HasErrors();
    }

    int result = tjDecompress2(m_JpegHandle, m_JpegFile, m_JpegSize, pixelsBuf, m_ScaledWidth, 0, m_ScaledHeight,
        pixelFormat, 0);

    if (result < 0) {
        tjFree(pixelsBuf);
        m_HasErrors = true;
        return !HasErrors();
    }

    MemoryUtils::Copy(reinterpret_cast<char*>(pixelsBuf), reinterpret_cast<char*>(buf), tjPixelSize[pixelFormat],
        m_ScaledHeight * m_ScaledWidth);

    tjFree(pixelsBuf);

    return !HasErrors();
}

bool JpegImageDecoder::GetFullImage(float* buf) {
    int pixelFormat = TJPF_RGB;

    auto pixelsBuf = tjAlloc(Width() * Height() * tjPixelSize[pixelFormat]);

    if (pixelsBuf == nullptr) {
        m_HasErrors = true;
        return !HasErrors();
    }

    int result = tjDecompress2(m_JpegHandle, m_JpegFile, m_JpegSize, pixelsBuf, Width(), 0, Height(), pixelFormat, 0);

    if (result < 0) {
        tjFree(pixelsBuf);
        m_HasErrors = true;
        return !HasErrors();
    }

    MemoryUtils::CopyFromUint8ToFloat(pixelsBuf, buf, 3, Width() * Height(), true);

    Converter::sRGB2RGB(buf, buf, Width() * 3, Height());

    tjFree(pixelsBuf);

    return !HasErrors();
}


int JpegImageDecoder::ScaledWidth() {
    return m_ScaledWidth;
}

int JpegImageDecoder::ScaledHeight() {
    return m_ScaledHeight;
}
