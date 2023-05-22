#include "Flux.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/Core/FluxImageProcessor.h"
#include "../engine/Core//Filters/BoxBlur.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

int main()
{
	std::string inputFilePath = "C:\\Users\\Artyom\\Downloads\\rectimage1024-1023.jpg";
	std::string outputFilePath = "C:\\Users\\Artyom\\Downloads\\rectimage1024-1023-Box-Blurred.jpg";
	ImageInput input = ImageInput(inputFilePath);

	input.Init();

	FluxImage* image =  input.DecodeImage();

	FluxImageProcessor::Init();

	FluxImage* processed = FluxImageProcessor::DebugProcessToLuma(image);
	
	Matrix<pixel_t> mat = Matrix<pixel_t>(image->Width, image->Height, (pixel_t*)processed->Pixels);
	Matrix<pixel_t> matBlur = Matrix<pixel_t>(image->Width, image->Height);

	auto watcher1 = BenchmarkHelper::StartWatch();

	BoxBlur::Blur(mat, matBlur, 10);

	BenchmarkHelper::ShowDurationFinal(watcher1, "Box blur took: ");

	FluxImage blurred = FluxImage();
	
	JpegImageEncoder encoder = JpegImageEncoder(matBlur.GetPointer(), matBlur.Width(), matBlur.Height(), 1);
	encoder.Init();
	encoder.FastSave(outputFilePath);

	ImageInput::FreeFluxImage(image);
	ImageInput::FreeFluxImage(processed);
	matBlur.Dispose();
	

	return 0;
}
