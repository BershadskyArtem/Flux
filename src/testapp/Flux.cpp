#include "Flux.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/Core/FluxImageProcessor.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

int main()
{
	Converter::Init();
	pixel_t r = 1;
	pixel_t g = 0;
	pixel_t b = 0;

	pixel_t okL = 0;
	pixel_t okA = 0;
	pixel_t okB = 0;

	pixel_t okC = 0;
	pixel_t okH = 0;


	Converter::RGB2OKLab(r,g,b, okL, okA, okB);
	Converter::OkLab2OkLCh(okL, okA, okB, okL, okC, okH);

	std::cout << okH << '\n';

	r = 0;
	g = 0;
	b = 1;


	Converter::RGB2OKLab(r, g, b, okL, okA, okB);

	std::cout << std::endl;
	//1 degree in oklch = 0.01747 in input i
	const double lchOffset = 0.453785;
	const double lchStep = 0.0174534;
	for (double i = lchOffset; i < lchOffset + 360 * lchStep; i+=lchStep)
	{
		double hueV = ((i + M_PI) / (2.0 * M_PI)) ;
		Converter::OkLCh2OkLab(0.6,0.2, i, okL, okA, okB);
		Converter::OKLab2RGB(okL, okA, okB, r, g, b);
		std::cout << hueV * 360 - 206 << " Hue \t" << i << " i \t" << r << " R \t" << g << " G\t" << b << " B\t" << '\n';
	}
	//Blue 0.5
	//Red 0 
	



	return 0;
}
