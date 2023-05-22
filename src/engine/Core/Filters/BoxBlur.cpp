#include "BoxBlur.h"

void BoxBlur::Blur(Matrix<pixel_t>& input, Matrix<pixel_t>& output, int radius)
{
	int inc = vfloat::size;
	int width = input.Width();
	int height = input.Height();
	pixel_t divisor = radius * 2 + 1.f;
	pixel_t* inputPixels = input.GetPointer();
	pixel_t* outputPixels = output.GetPointer();

	if (radius == 0) {
		for (int m = 0; m < width * height; m++)
		{
			outputPixels[m] = inputPixels[m];
		}
		return;
	}



	//Horizontal pass

#pragma omp parallel for
	for (int y = 0; y < height; y++)
	{
		int startOfLine = y * width;
		int x = 0;

		pixel_t accumulator = 0;

		//Initialize accumulator, shifted one to the left so we can start with subtracting and adding early
		for (int leftBorder = -radius - 1;  leftBorder < radius; ++leftBorder)
		{
			int idx = x + leftBorder;

			if (idx < 0)
				idx = std::abs(idx + 1);

			accumulator += inputPixels[startOfLine + idx];
		}


		//Left overhang
		for (;x <= radius; ++x)
		{	
			int leftBorder = x - radius - 1;
			leftBorder = std::abs(leftBorder + 1);

			int rightBorder = x + radius;

			accumulator = accumulator - inputPixels[startOfLine + leftBorder] + inputPixels[startOfLine + rightBorder];

			outputPixels[startOfLine + x] = accumulator / divisor;
		}

		//Central
		for (;x < width - radius - 1; ++x) {
			int leftBorder = x - radius - 1;
			int rightBorder = x + radius;
			accumulator = accumulator - inputPixels[startOfLine + leftBorder] + inputPixels[startOfLine + rightBorder];
			outputPixels[startOfLine + x] = accumulator / divisor;
		}

		//Right
		for (;x < width; ++x) {
			int leftBorder = x - radius - 1;
			int rightBorder = x + radius;

			if (rightBorder >= width - 1) {
				rightBorder = width - (rightBorder - (width - 1));
			}
			accumulator = accumulator - inputPixels[startOfLine + leftBorder] + inputPixels[startOfLine + rightBorder];
			outputPixels[startOfLine + x] = accumulator / divisor;
		}
	}

	//Vertical pass
#pragma omp parallel for
	for (int x = 0; x < width; ++x)
	{
		int y = 0;
		pixel_t accumulator = 0;
		pixel_t* verticalBuffer = new pixel_t[height];

		//Initialize accumulator
		for (int upperBorder = -radius - 1; upperBorder < radius; upperBorder++)
		{
			int idy = y + upperBorder;

			if (idy < 0)
				idy = std::abs(idy + 1);
			accumulator += outputPixels[idy * width + x];
		}

		//Upper overhang
		for (; y <= radius; ++y)
		{
			int previousElement = y - radius - 1;
			previousElement = std::abs(previousElement + 1);
			int nextElement = y + radius;

			accumulator = accumulator - outputPixels[previousElement * width + x] + outputPixels[nextElement * width + x];
			verticalBuffer[y] = accumulator / divisor;
		}

		//Center
		for (;y < height - radius - 1; ++y) {
			int previousElement = y - radius - 1;
			int nextElement = y + radius;
			accumulator = accumulator - outputPixels[previousElement * width + x] + outputPixels[nextElement * width + x];
			verticalBuffer[y] = accumulator / divisor;
		}
		//Bottom overhang
		for (; y < height; ++y) {
			int previousElement = y - radius - 1;
			int nextElement = y + radius;


			if (nextElement >= width - 1) {
				nextElement = width - (nextElement - (width - 1));
			}
			accumulator = accumulator - outputPixels[previousElement * width + x] + outputPixels[nextElement * width + x];
			verticalBuffer[y] = accumulator / divisor;
		}

		for (int yBufIdx = 0; yBufIdx < height; ++yBufIdx) {
			outputPixels[yBufIdx * width + x] = verticalBuffer[yBufIdx];
		}
		delete[] verticalBuffer;
	}

}
