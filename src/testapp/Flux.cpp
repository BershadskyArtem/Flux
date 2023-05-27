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
	ColorLUTConverter::Init();
	FluxImageProcessor::Init();
	std::string inputFilePath = "C:\\Users\\Artyom\\Downloads\\Lena.jpg";
	std::string outputFilePath = "C:\\Users\\Artyom\\Downloads\\Lena-Pipeline-Test.jpg";
	std::string outputFilePath2 = "C:\\Users\\Artyom\\Downloads\\Lena-RGB.jpg";
	ImageInput input = ImageInput(inputFilePath);

	input.Init();

	FluxImage* image =  input.DecodeImage();

	JpegImageEncoder encoder1 = JpegImageEncoder((pixel_t*)image->Pixels, image->Width, image->Height, 3);
	encoder1.Init();
	encoder1.FastSave(outputFilePath2);

	//FluxImage* processed = FluxImageProcessor::DebugProcessToLuma(image);
	
	//Matrix<pixel_t> mat = Matrix<pixel_t>(image->Width, image->Height, (pixel_t*)processed->Pixels);

	ProcessSettings set = ProcessSettings();
	set.Layers = new ProcessSettingsLayer[1]{};
	set.Layers[0] = ProcessSettingsLayer();
	set.Layers[0].Crop.LeftUpX = 0;
	set.Layers[0].Crop.LeftUpY = 0;
	set.Layers[0].Crop.RightDownX = 512;
	set.Layers[0].Crop.RightDownY = 512;
	set.Layers[0].Denoise.Chrominance = 0;
	set.Layers[0].Denoise.Luminance = 0;
	set.Layers[0].Texture.Amount = 1;
	set.Layers[0].Clarity.Amount = 1;
	set.ChangedLayer = 0;
	set.ChangedStage = ProcessingStage::Input;

	ProcessingCache* cache = FluxImageProcessor::PreProcess(image, nullptr);
	FluxImage* processedImage = FluxImageProcessor::FastProcessToBitmap(cache, &set);
		
	JpegImageEncoder encoder = JpegImageEncoder((pixel_t*)processedImage->Pixels, processedImage->Width, processedImage->Height, 3);
	encoder.Init();
	encoder.FastSave(outputFilePath);
	ImageInput::FreeFluxImage(image);
	ImageInput::FreeFluxImage(processedImage);

	return 0;
}
