#include "Flux.h"
#include "../engine/Color/Converter.h"
#include "../engine/infrastructure/LUTf3d.h"
#include <chrono>
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


	int lutSize = 32;
	LUTf3d lut = LUTf3d();
	lut.Init(lutSize, lutSize, lutSize, 0.f);
	
	float factor = 255.f / lutSize;

	auto now = chrono::high_resolution_clock::now();

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
				lut.SetValue((0.2126f * r + 0.7152f * g + 0.0722f * b), i, j, k);
			}
		}
	}

	auto newNow = chrono::high_resolution_clock::now();

	std::cout << chrono::duration_cast<chrono::milliseconds>(newNow - now).count() << std::endl;

	r = 0.5f;
	g = 0.5f;
	b = 0.5f;

	std::cout << (0.2126f * r + 0.7152f * g + 0.0722f * b) << std::endl;
	std::cout << lut.Get01(r, g, b)/255.f << std::endl;


	std::cout << "et" << std::endl;

	lut.Dispose();

	return 0;
}
