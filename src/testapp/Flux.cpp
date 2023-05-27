#include "Flux.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/Core/FluxImageProcessor.h"
#include "../engine/Core//Filters/BoxBlur.h"
#include "../engine/Color/FluxColorMath.h"


#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

int main()
{
	Converter::Init();
	ColorLUTConverter::Init();

	int rIn = 0;
	int gIn = 0;
	int bIn = 0;

	std::cin >> rIn;
	std::cin >> gIn;
	std::cin >> bIn;
	
	pixel_t r = rIn / 255.0f;
	pixel_t g = gIn / 255.0f;
	pixel_t b = bIn / 255.0f;
	
	pixel_t l1, l2;
	pixel_t a1, c2;
	pixel_t b1, h2;
	
	Converter::RGB2OKLab(r, g, b, l1, a1, b1);
	Converter::OkLab2OkLCh(l1, a1, b1, l2, c2, h2);
	
	std::cout << FluxColorMath::NormalizeOkHue(h2) << std::endl;

	pixel_t hueT = h2;

	
	ColorSelectionBorder border = ColorSelectionBorder();
	border.HueLeft = 260;
	border.HueRight = 40;
	border.SaturationLeft = 0;
	border.SaturationRight = 1.0;
	border.Smoothness = 10;

	std::cout << FluxColorMath::IsInsideHueChrominanceBoundary(hueT, 0.5f, border) << std::endl;

	return 0;
}
