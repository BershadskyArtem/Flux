#pragma once
#include "../../infrastructure/FLXDefines.h"
#include "../../infrastructure/Matrix.h"

class BoxBlur {
public:
	static void Blur(Matrix<pixel_t> &input, Matrix<pixel_t>& output, int radius);
};