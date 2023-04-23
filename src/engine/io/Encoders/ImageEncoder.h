#pragma once

#include <string>
#include "../../infrastructure/InteropTypes.h"
#include "../../infrastructure/FLXDefines.h"
#include "../../infrastructure/Matrix.h"

class AbstractImageEncoder {
protected:
    std::string m_fileName;
    bool m_HasErrors = false;
    int m_ColorComponents = 0;
    int m_Width;
    int m_Height;
    void* m_pixels;
    PixelType m_PixelType = PixelType::Float;
public:
    AbstractImageEncoder(pixel_t* pixels, int width, int height, int colorComponents);
    AbstractImageEncoder(byte_t* pixels, int width, int height, int colorComponents);
    AbstractImageEncoder(Matrix<pixel_t> &mat);

    virtual bool Init() = 0;
    virtual bool FastSave(std::string filePath) = 0;
    virtual bool HasErrors();
    virtual ~AbstractImageEncoder();
};