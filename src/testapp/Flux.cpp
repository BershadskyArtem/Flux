#include "Flux.h"

using namespace std;

int main()
{
	float hiCoeffs[] = {
		-0.48296291314469025,
		0.836516303737469,
		-0.22414386804185735,
		-0.12940952255092145
	};

	float loCoeffs[] = {
		-0.12940952255092145,
		0.22414386804185735,
		0.836516303737469,
		0.48296291314469025
	};

	const int filterSize = 4;
	const int inputLength = 8;

	float input[] = { 0, 1, 5, 1, 0, 6, 0, 1 };
	float hiOutput[inputLength] = {};
	float lowOutput[inputLength] = {};

	for (int i = 2; i < inputLength + filterSize - 2; i += 2)
	{
		for (int j = 0; j < filterSize; j++)
		{
			int index = i - j;
			if (index < 0) {
				index += inputLength;
			}
			if (index > inputLength - 1) {
				index -= inputLength;
			}
			hiOutput[(i / 2) - 1] += input[index] * hiCoeffs[j];
			lowOutput[(i / 2) - 1] += input[index] * loCoeffs[j];
		}
	}

	for (int i = 0; i < inputLength; i++)
	{
		std::cout << hiOutput[i] << std::endl;
		std::cout << lowOutput[i] << std::endl;
		std::cout << std::endl;
	}


	return 0;
}
