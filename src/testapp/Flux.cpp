#include "Flux.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/Core/FluxImageProcessor.h"

using namespace std;

int main()
{
	std::string fileInput = "C:\\Users\\Artyom\\Downloads\\rectimage1023-1023.jpg";
	std::string fileOutput = "C:\\Users\\Artyom\\Downloads\\waveleted.jpg";

	ImageInput input = ImageInput(fileInput);
	input.Init();
	FluxImage* image = input.DecodeImage();

	FluxImageProcessor processor = FluxImageProcessor();
	
	auto timeProcess = BenchmarkHelper::StartWatch();
	auto processed = processor.Process(image, nullptr);
	BenchmarkHelper::ShowDurationFinal(timeProcess, "Time for processing took");


	ImageInput::FreeFluxImage(image);

	Matrix<pixel_t> mat = Matrix<pixel_t>(processed->Width, processed->Height, (pixel_t*)(processed->Pixels));

	Daubechies4* db4 = new Daubechies4();

	FluxWaveletDenoising denoiser = FluxWaveletDenoising(db4);

	auto timestamp = BenchmarkHelper::StartWatch();
	std::vector<WaveletImage<pixel_t>> waveletImages = denoiser.Wavedec(mat);
	BenchmarkHelper::ShowDurationFinal(timestamp, "Forward 2d dwt");
	std::cout << waveletImages.size() << '\n';


	std::vector<pixel_t> thresholds = std::vector<pixel_t>();

	pixel_t mul = 0.4f;

	thresholds.push_back(mul * 50.0f  / 100.f);
	thresholds.push_back(mul * 75.0f  / 100.f);
	thresholds.push_back(mul * 50.0f  / 100.f);
	thresholds.push_back(mul * 75.0f  / 100.f);
	thresholds.push_back(mul * 75.0f  / 100.f);
	thresholds.push_back(mul * 25.0f  / 100.f);
	thresholds.push_back(mul * 0.0f  / 100.f);
	//thresholds.push_back(0.0f  / 100.f);


	auto timestampDenoise = BenchmarkHelper::StartWatch();

	std::vector<WaveletImage<pixel_t>> waveletImagesDenoised = denoiser.ApplyDenoising(waveletImages, thresholds);

	BenchmarkHelper::ShowDurationFinal(timestampDenoise, "Denoising applied");

	auto timestamp2 = BenchmarkHelper::StartWatch();
	//Matrix<pixel_t> waveletedMatrix = denoiser.Idwt2d(waveletImage);
	Matrix<pixel_t> waveletedMatrix = denoiser.Waveinv(waveletImagesDenoised);
	//waveletImage.Dispose();
	BenchmarkHelper::ShowDurationFinal(timestamp2, "Inverse 2d dwt");

	for (size_t i = 0; i < waveletImages.size(); i++)
	{
		waveletImages[i].Dispose(); 
		waveletImagesDenoised[i].Dispose();
	}
	waveletImages.clear();

	ImageInput::FreeFluxImage(processed);
	JpegImageEncoder encoder = JpegImageEncoder(waveletedMatrix);
	encoder.Init();
	encoder.FastSave(fileOutput);
	
	waveletedMatrix.Dispose();
	db4->Dispose();
	delete db4;
	return 0;
}
