#include "Flux.h"
#include "../engine/Color/Converter.h"

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

	std::cout << r << std::endl;
	std::cout << g << std::endl;
	std::cout << b << std::endl;

	return 0;
}
