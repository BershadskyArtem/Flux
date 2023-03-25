#include "FluxImageProcessor.h"

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
