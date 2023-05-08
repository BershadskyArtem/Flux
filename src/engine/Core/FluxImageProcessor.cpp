#include "FluxImageProcessor.h"
#include "../infrastructure/Matrix.h"

FluxImage* FluxImageProcessor::Process(FluxImage* image, ProcessSettings* settings)
{
	if (!ColorLUTConverter::IsInitialized()) {
		ColorLUTConverter::Init();
	}

	pixel_t* rgbImage = (pixel_t*)image->Pixels;
	
	int width = image->Width;
	int height = image->Height;

	pixel_t* rChannel = new pixel_t[width * height];
	pixel_t* gChannel = new pixel_t[width * height];
	pixel_t* bChannel = new pixel_t[width * height];

	PixelsHelper::Deinterleave(rgbImage, rChannel, gChannel, bChannel, width, height);

	ColorLUTConverter::ConvertToLab(rChannel, gChannel, bChannel, rChannel, gChannel, bChannel, width, height);

	//Just rename
	pixel_t* lChannel = rChannel;
	pixel_t* aChannel = gChannel;
		
	//Do not delete rChannel because it is an lChannel that we export for now
	//delete[] rChannel;
	delete[] gChannel;
	delete[] bChannel;

	FluxImage* result = new FluxImage();
	result->Pixels = lChannel;
	result->Type = PixelType::Float;
	result->Width = width;
	result->Height = height;
	result->ChannelsCount = 1;

	return result;
}

FluxImage* FluxImageProcessor::PreProcess(FluxImage* image, ProcessSettings* settings)
{
	//Matrix<pixel_t> image = Matrix<pixel_t>(image->Width, image->Height, (pixel_t*)image->Pixels);
	
	//Deinterleave

	//Color manage 

	//Wavelet decomposition

	//Fill caches

	//Return

	return nullptr;
}

FluxImage* FluxImageProcessor::FastProcess(FluxImage* image, ProcessSettings* settings)
{
	//Check performed operation compared to previous fast process call
	
	//TODO: Create mask drawing api

	//Get latest cache

	//Apply changes from the latest cache point

	/*
		White balance? After Skew or in LUT?
		
		Pipeline:

		--Before all
		Crop 
		Skew
		

		--Foreach layer
		
		--Prepare 
		White balance (Only on first layer)
		To Lab

		--Spatial
		Denoise
		Dehaze
		Clarity
		Texture

		--LUT
		Brightness - HDR
		Contrast
		Levels \
				Can be swaped
		Curve  / 
		HSL
		Toning

		--Linear interpolate between previous and current layer
	*/

	return nullptr;
}
