#include <chrono>
#include "Flux.h"
#include "../engine/Color/Converter.h"
#include "../engine/infrastructure/LUTf3d.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/PixelsHelper.h"

using namespace std;


int main()
{
	Converter::Init();

	float r = 0.f;
	float g = 0.5f;
	float b = 0.f;


	float l = 0.f;
	float a = 0.f;
	//float b = 0.f;

	float c = 0.f;
	float h = 0.f;

	Converter::RGB2OKLab(r, g, b, l, a, b);

	Converter::OkLab2OkLCh(l, a, b, l, c, h);

	Converter::OkLCh2OkLab(l, c, h, l, a, b);

	Converter::OKLab2RGB(l, a, b, r, g, b);


	int lutSize = 64;
	LUTf3d lut = LUTf3d();
	lut.Init(lutSize, lutSize, lutSize, 0.f);

	float factor = 255.f / lutSize;



#pragma omp parallel for
	for (int i = 0; i < lutSize; i++)
	{
		for (int j = 0; j < lutSize; j++)
		{
			for (int k = 0; k < lutSize; k++)
			{
				float r = i * factor;
				float g = j * factor;
				float b = k * factor;
				lut.SetValue((0.2126f * r + 0.7152f * g + 0.0722f * b)/255.f, i, j, k);
			}
		}
	}



	r = 0.5f;
	g = 0.5f;
	b = 0.5f;


	vfloat rV = 0.5f;
	vfloat gV = 0.5f;
	vfloat bV = 0.5f;
	vfloat divisor = 255.f;

	std::string filePath = "C:\\Users\\Artyom\\Downloads\\DSC00213.JPG";

	ImageInput decoder = ImageInput(filePath);
	decoder.Init();
	FluxImage* image = decoder.DecodeImage();

	int width = image->Width;
	int height = image->Height;
	int size = width * height;
	float* pixels = (float*)(image->Pixels);
	float* wbPixels = new float[size] {};
	int inc = vfloat::size;

	auto now = chrono::high_resolution_clock::now();

#pragma omp parallel for
	for (int i = 0; i < size; i += 1)
	{
		int rIndex = i * 3;
		int gIndex = i * 3 + 1;
		int bIndex = i * 3 + 2;

		float rP = pixels[rIndex];
		float gP = pixels[gIndex];
		float bP = pixels[bIndex];

		wbPixels[i] = lut.Get01(rP, gP, bP);
	}

	auto newNow = chrono::high_resolution_clock::now();
	std::cout << "LUT applied" << std::endl;
	std::cout << chrono::duration_cast<chrono::milliseconds>(newNow - now).count() << std::endl;

	std::string filePathOut = "C:\\Users\\Artyom\\Downloads\\FirstTest01.JPG";

	JpegImageEncoder encoder = JpegImageEncoder(wbPixels, width, height, 1);
	encoder.Init();
	encoder.FastSave(filePathOut);

	std::cout << (0.2126f * r + 0.7152f * g + 0.0722f * b) << std::endl;
	std::cout << lut.Get01(r, g, b) / 255.f << std::endl;
	std::cout << lut.Get01(rV, gV, bV) / divisor << std::endl;

	std::cout << "et" << std::endl;

	lut.Dispose();

	ImageInput::FreeFluxImage(image);

	return 0;
}
