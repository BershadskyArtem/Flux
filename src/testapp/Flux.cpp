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

	std::cout << std::setfill('0') << std::setw(5);
	std::cout << std::setprecision(5); //<< std::fixed;

	pixel_t* inputs = new pixel_t[]{
		0.0117,
		0.19,
		0.368,
		0.547,
		0.725,
		0.903,
		1.08,
		1.26
	};

	//auto mmn = wv.Dwt(inputs, 8);
	//std::cout << "CH coefs" << '\n';
	//for (int i = 0; i < mmn.Length; i++)
	//{
	//	std::cout << mmn.Hi[i] << '\t';
	//	//std::cout << '\n';
	//}
	//std::cout << '\n';


	Matrix<pixel_t> pixels = Matrix<pixel_t>(8, 8, input);
	pixels.Print();

	auto wavedec = wv.Dwt2d(pixels);
	
	//std::cout << "CA" << '\n';
	//wavedec.CA->Print();
	//std::cout << "CD" << '\n';
	//wavedec.CD->Print();
	//std::cout << "CH" << '\n';
	//wavedec.CH->Print();
	//std::cout << "CV" << '\n';
	//wavedec.CV->Print();


	auto mat = wv.Idwt2d(wavedec);
	std::cout << '\n';
	std::cout << "Restored" << '\n';
	mat.Print();
	

	return 0;
}
