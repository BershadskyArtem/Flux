﻿#include "Flux.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/Core/FluxImageProcessor.h"

using namespace std;

int main()
{
	std::string fileInput = "C:\\Users\\Artyom\\Downloads\\rectimage.jpg";
	std::string fileOutput = "C:\\Users\\Artyom\\Downloads\\waveleted.jpg";

	ImageInput input = ImageInput(fileInput);
	input.Init();
	FluxImage* image = input.DecodeImage();

	FluxImageProcessor processor = FluxImageProcessor();
	
	auto processed = processor.Process(image, nullptr);
	ImageInput::FreeFluxImage(image);

	Matrix<pixel_t> mat = Matrix<pixel_t>(processed->Width, processed->Height, (pixel_t*)(processed->Pixels));

	Daubechies4* db4 = new Daubechies4();

	FluxWaveletDenoising denoiser = FluxWaveletDenoising(db4);

	auto timestamp = BenchmarkHelper::StartWatch();
	WaveletImage<pixel_t> waveletImage = denoiser.Dwt2d(mat);
	BenchmarkHelper::ShowDurationFinal(timestamp, "Forward 2d dwt");
	
	auto timestamp2 = BenchmarkHelper::StartWatch();
	Matrix<pixel_t> waveletedMatrix = denoiser.Idwt2d(waveletImage);
	waveletImage.Dispose();
	BenchmarkHelper::ShowDurationFinal(timestamp2, "Inverse 2d dwt");

	ImageInput::FreeFluxImage(processed);

	JpegImageEncoder encoder = JpegImageEncoder(waveletedMatrix);
	encoder.Init();
	encoder.FastSave(fileOutput);
	
	waveletedMatrix.Dispose();
	db4->Dispose();
	delete db4;
	return 0;
}
