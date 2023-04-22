#include "Flux.h"

using namespace std;

int main()
{
	int inputSize = 64;
	pixel_t* input = new pixel_t[]
	{
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64
	};
	Daubechies4* wvData = new Daubechies4();

	FluxWaveletDenoising wv = FluxWaveletDenoising(wvData);

	Matrix<pixel_t> pixels = Matrix<pixel_t>(8,8, input);

	for (size_t x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			std::cout << pixels.at(y,x) << '\t';
		}
		std::cout << '\n';
	}


	for (int i = 0; i < 8; i++)
	{
		auto vc = pixels.GetRow(i);

		for (int x = 0; x < 8; x++)
		{
			std::cout << vc[x] << '\t';	
		}
			std::cout << '\n';
	}

	for (int i = 0; i < 8; i++)
	{
		auto vc = pixels.GetColumn(i);

		for (int x = 0; x < 8; x++)
		{
			std::cout << vc[x] << '\t';
			
		}
		std::cout << '\n';
	}


	auto wavedec = wv.Dwt2d(pixels);
	
	for (int i = 0; i < wavedec.Height; i++)
	{
		for (int x = 0; x < wavedec.Width; x++)
		{
			std::cout << wavedec.HH[i * wavedec.Height + x] << '\t';
		}

		std::cout << '\n';
	}


	
	return 0;
}
