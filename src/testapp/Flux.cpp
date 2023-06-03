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
	ColorLUTConverter::Init();
	FluxImageProcessor::Init();
	//std::string inputFilePath = "C:\\Users\\Artyom\\Downloads\\rectimage1023-1024.jpg";
	std::string inputFilePath = "C:\\Users\\Artyom\\Desktop\\IMG_9409-NoDenoising.jpg";

	//std::string outputFilePath = "C:\\Users\\Artyom\\Downloads\\rectimage1023-1024-Pipeline.jpg";
	std::string outputFilePath = "C:\\Users\\Artyom\\Desktop\\IMG_9409-MyDenoising.jpg";
	ImageInput input = ImageInput(inputFilePath);

	input.Init();

	FluxImage* image = input.DecodeImage();

	//JpegImageEncoder encoder1 = JpegImageEncoder((pixel_t*)image->Pixels, image->Width, image->Height, 3);
	//encoder1.Init();
	//encoder1.FastSave(outputFilePath2);

	//FluxImage* processed = FluxImageProcessor::DebugProcessToLuma(image);

	//Matrix<pixel_t> mat = Matrix<pixel_t>(image->Width, image->Height, (pixel_t*)processed->Pixels);
	std::cout << std::endl;
	ProcessSettings set = ProcessSettings();
	set.Layers = new ProcessSettingsLayer[1]{};
	set.Layers[0] = ProcessSettingsLayer();
	set.Layers[0].Crop.LeftUpX = 0;
	set.Layers[0].Crop.LeftUpY = 0;
	set.Layers[0].Crop.RightDownX = image->Width;
	set.Layers[0].Crop.RightDownY = image->Height;
	set.Layers[0].Denoise.Chrominance = 0;

	set.Layers[0].Resize.ResizeToX = 500;
	set.Layers[0].Resize.ResizeToY = 500;
	set.Layers[0].Resize.Mode = 1;
	set.Layers[0].Resize.Scale = 0.25f;

	//Max 40
	set.Layers[0].Denoise.Luminance = 0;
	set.Layers[0].Denoise.Chrominance = 75;
	set.Layers[0].Texture.Amount = 100;
	set.Layers[0].Clarity.Amount = 200;
	set.ChangedLayer = 0;
	set.ChangedStage = ProcessingStage::Input;

	auto preProcessTime = BenchmarkHelper::StartWatch();
	ProcessingCache* cache = FluxImageProcessor::PreProcess(image, nullptr);
	BenchmarkHelper::ShowDurationFinal(preProcessTime, "Pre-process time took... ");

	auto fastProcessFromGround = BenchmarkHelper::StartWatch();
	FluxImage* processedImage = FluxImageProcessor::FastProcessToBitmap(cache, &set);
	BenchmarkHelper::ShowDurationFinal(fastProcessFromGround, "Fast processing from ground up took... ");

	set.ChangedStage = ProcessingStage::Lut;
	auto lutOnlyStage = BenchmarkHelper::StartWatch();
	FluxImage* processedImage2 = FluxImageProcessor::FastProcessToBitmap(cache, &set);
	BenchmarkHelper::ShowDurationFinal(lutOnlyStage, "Lut only fast processing stage took... ");

	JpegImageEncoder encoder = JpegImageEncoder((pixel_t*)processedImage2->Pixels, processedImage2->Width, processedImage2->Height, 3);
	encoder.Init();
	encoder.FastSave(outputFilePath);
	ImageInput::FreeFluxImage(image);
	ImageInput::FreeFluxImage(processedImage);
	ImageInput::FreeFluxImage(processedImage2);
	FluxImageProcessor::DisposeAllCache(cache);

	return 0;
}
