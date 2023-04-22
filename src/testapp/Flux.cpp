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

	auto wavedec = wv.Dwt2d(pixels);
	std::cout << "CH coefs" << '\n';
	for (int i = 0; i < wavedec.Height; i++)
	{
		for (int x = 0; x < wavedec.Width; x++)
		{
			std::cout << wavedec.CH->at(i * wavedec.Height + x) << '\t';
		}

		std::cout << '\n';
	}

	std::cout << "CA coefs" << '\n';
	for (int i = 0; i < wavedec.Height; i++)
	{
		for (int x = 0; x < wavedec.Width; x++)
		{
			std::cout << wavedec.CA->at(i * wavedec.Height + x) << '\t';
		}

		std::cout << '\n';
	}

	std::cout << "CV coefs" << '\n';
	for (int i = 0; i < wavedec.Height; i++)
	{
		for (int x = 0; x < wavedec.Width; x++)
		{
			std::cout << wavedec.CV->at(i * wavedec.Height + x) << '\t';
		}

		std::cout << '\n';
	}

	std::cout << "CD coefs" << '\n';
	for (int i = 0; i < wavedec.Height; i++)
	{
		for (int x = 0; x < wavedec.Width; x++)
		{
			std::cout << wavedec.CD->at(i * wavedec.Height + x) << '\t';
		}

		std::cout << '\n';
	}




	auto mat = wv.Idwt2d(wavedec);

	std::cout << "Restored" << '\n';

	for (int i = 0; i < mat.Height(); i++)
	{
		for (int j = 0; j < mat.Width(); j++)
		{
			std::cout << mat.at(i,j) << '\t';
		}
		std::cout << '\n';
	}


	return 0;
}
