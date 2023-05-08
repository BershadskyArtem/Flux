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

ProcessingCache* FluxImageProcessor::PreProcess(FluxImage* image, ProcessSettings* settings)
{
	pixel_t* pixelsR = new pixel_t[image->Width * image->Height];
	pixel_t* pixelsG = new pixel_t[image->Width * image->Height];
	pixel_t* pixelsB = new pixel_t[image->Width * image->Height];

	//Deinterleave
	PixelsHelper::Deinterleave((pixel_t*)image->Pixels, pixelsR, pixelsG, pixelsB, image->Width, image->Height);

	Matrix<pixel_t> rMat = Matrix<pixel_t>(image->Width, image->Height, pixelsR);
	Matrix<pixel_t> gMat = Matrix<pixel_t>(image->Width, image->Height, pixelsG);
	Matrix<pixel_t> bMat = Matrix<pixel_t>(image->Width, image->Height, pixelsB);

	//Color manage 
	//I don't have time to handle any camera out there

	//Fill caches
	InternalImageData* data = new InternalImageData();
	data->RPixels = pixelsR;
	data->GPixels = pixelsG;
	data->BPixels = pixelsB;
	data->Width = image->Width;
	data->Height = image->Height;
	ProcessingCache* cache = ConstructCache(data);
	return cache;
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

ProcessingCache* FluxImageProcessor::ConstructCache(InternalImageData* image)
{
	ProcessingCache* cache = new ProcessingCache();
	cache->LayersCount = 1;
	cache->Layers = new ProcessingLayerCache[1]{};
	cache->Layers[0] = ProcessingLayerCache();
	ProcessingLayerCache& firstLayer = cache->Layers[0];
	firstLayer.CachesCount = 1;
	firstLayer.Caches = new ProcessingCacheEntry[1]{};
	ProcessingCacheEntry& firstEntry = firstLayer.Caches[0];
	firstEntry.Stage = ProcessingStage::Initial;
	firstEntry.CachesCount = 1;
	firstEntry.Caches = image;
	firstEntry.PreviousSettings = DefaultSettings();
	return cache;
}

ProcessSettings* FluxImageProcessor::DefaultSettings()
{
	ProcessSettings* settings = new ProcessSettings();
	settings->Layers = new ProcessSettingsLayer[1]{};
	settings->LayersCount = 1;
	ProcessSettingsLayer& layer = settings->Layers[0];

	return settings;
}

FluxImage* FluxImageProcessor::FastProcessToBitmap(ProcessingCache* cache, ProcessSettings* settings)
{
	ProcessingStage correctionStage = settings->ChangedStage;
	int changedLayer = settings->ChangedLayer;

	//Dispose all useless layers
	for (int i = cache->LayersCount; i > changedLayer; i--)
	{
		ProcessingLayerCache& layer = cache->Layers[i];
		DisposeLayer(&layer);
	}


	ProcessingLayerCache& currentLayer = cache->Layers[settings->ChangedLayer];

	//We intend that order of the caches are increasing
	//Order is from first to last correction
	//So we get rid of everything we don't need until we encounter our cache that might be useful
	for (int i = currentLayer.CachesCount; i > correctionStage; i--)
	{
		DisposeStageCache(&currentLayer.Caches[i]);
	}

	
	for (int i = correctionStage; i < length; i++)
	{

	}




	return nullptr;
}

void FluxImageProcessor::DisposeLayer(ProcessingLayerCache* layer)
{
	for (int i = 0; i < layer->CachesCount; i++)
	{
		ProcessingCacheEntry& entry = layer->Caches[i];
		DisposeStageCache(&entry);
	}
}

void FluxImageProcessor::DisposeStageCache(ProcessingCacheEntry* entry)
{
	switch (entry->Stage)
	{
	case ProcessingStage::NotSet:
		return;

	case ProcessingStage::Initial:
		return;

	case ProcessingStage::Resize:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::Rotate:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::Crop:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::Denoise:
		DenoiseCache* cache = (DenoiseCache*)entry->Caches;
		for (int i = 0; i < cache->WaveletDecCount; i++)
		{
			WaveletImage<pixel_t> waveletImage = cache->WaveletDec[i];
			waveletImage.Dispose();
		}
		delete cache;
		return;

	case ProcessingStage::Dehaze:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::Clarity:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::Texture:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	case ProcessingStage::LUT:
		InternalLabImage* image = (InternalLabImage*)entry->Caches;
		delete[] image->LPixels;
		delete[] image->APixels;
		delete[] image->BPixels;
		delete image;
		return;

	default:
		return;
	}

}
