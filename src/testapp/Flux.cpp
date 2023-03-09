#include <chrono>
#include "Flux.h"
#include "../engine/Color/Converter.h"
#include "../engine/infrastructure/LUTf3d.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/PixelsHelper.h"
#include "../engine/infrastructure/BenchmarkHelper.h"

using namespace std;


int main()
{
	Converter::Init();
	int lutSize = 32;

	LUTf3d lutR = LUTf3d();
	LUTf3d lutG = LUTf3d();
	LUTf3d lutB = LUTf3d();
	
	
	lutR.Init(lutSize, lutSize, lutSize, 0.f);
	lutG.Init(lutSize, lutSize, lutSize, 0.f);
	lutB.Init(lutSize, lutSize, lutSize, 0.f);

	float factor = 255.f / lutSize;

	auto timeToFillLut = BenchmarkHelper::StartWatch();

#pragma omp parallel for
	for (int i = 0; i < lutSize; i++)
	{
		for (int j = 0; j < lutSize; j++)
		{
			for (int k = 0; k < lutSize; k++)
			{
				float r = i * factor / 255.f;
				float g = j * factor / 255.f;
				float b = k * factor / 255.f;

				float l = (0.2126f * r + 0.7152f * g + 0.0722f * b);
				lutR.SetValue(l, i, j, k);
				lutG.SetValue(l, i, j, k);
				lutB.SetValue(l, i, j, k);
			}
		}
	}

	BenchmarkHelper::ShowDurationFinal(timeToFillLut, "Time to fill LUT was:");

	//std::string filePath = "C:\\Users\\Artyom\\Downloads\\DSC00213.JPG";
	std::string filePath = "C:\\Users\\Artyom\\Downloads\\sample1.jpeg";

	ImageInput decoder = ImageInput(filePath);
	decoder.Init();
	FluxImage* image = decoder.DecodeImage();

	int width = image->Width;
	int height = image->Height;
	int size = width * height;
	float* pixels = (float*)(image->Pixels);
	float* wbPixels = new float[size] {};
	int inc = vfloat::size;
	float* rInitial = new float[size] {};
	float* gInitial = new float[size] {};
	float* bInitial = new float[size] {};

	auto timeToDeinterleave = BenchmarkHelper::StartWatch();

	PixelsHelper::Deinterleave(pixels, rInitial, gInitial, bInitial, width, height);

	BenchmarkHelper::ShowDurationFinal(timeToDeinterleave, "Time to deinterleave was:");

	auto timeToApplyLUT = BenchmarkHelper::StartWatch();

	LUTf3d::ApplyLUTs(rInitial, gInitial,bInitial, rInitial, gInitial, bInitial, lutR, lutG, lutB, width, height);

	BenchmarkHelper::ShowDurationFinal(timeToApplyLUT, "Time to apply LUT was:");

	auto timeToInterleave = BenchmarkHelper::StartWatch();

	PixelsHelper::Interleave3(rInitial, gInitial, bInitial, pixels, width, height);

	BenchmarkHelper::ShowDurationFinal(timeToInterleave, "Time to interleave was");

	std::string filePathOut = "C:\\Users\\Artyom\\Downloads\\FirstTest01.JPG";

	JpegImageEncoder encoder = JpegImageEncoder(pixels, width, height, 3);
	encoder.Init();
	encoder.FastSave(filePathOut);

	ImageInput::FreeFluxImage(image);
	lutR.Dispose();

	delete[] rInitial;
	delete[] gInitial;
	delete[] bInitial;

	return 0;
}
