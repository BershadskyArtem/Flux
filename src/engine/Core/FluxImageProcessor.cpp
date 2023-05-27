#include "FluxImageProcessor.h"
#include "ImageOperations/Implementations/CropImageOperation.h"
#include "ImageOperations/Implementations/DenoiseImageOperation.h"
#include "../infrastructure/BenchmarkHelper.h"
#include "ImageOperations/Implementations/DetailsImageOperation.h"
#include "ImageOperations/Implementations/WaveletDecomposeImageOperation.h"
#include "ImageOperations/Implementations/WaveletComposeImageOperation.h"
#include "ImageOperations/Implementations/InputImageOperation.h"
#include "ImageOperations/Implementations/LutImageOperation.h"

#include "../io/Encoders/JpegImageEncoder.h"

std::vector<BaseImageOperation*> FluxImageProcessor::s_Operations = std::vector<BaseImageOperation*>();

void FluxImageProcessor::Init()
{
	//BaseImageOperation* op = ;

	s_Operations.push_back(new InputImageOperation());
	//s_Operations.push_back(new CropImageOperation());
	//s_Operations.push(new ResizeImageOperation());
	//s_Operations.push_back(new WaveletDecomposeImageOperation());
	//s_Operations.push_back(new DenoiseImageOperation());
	//s_Operations.push(new DehazeImageOperation());
	//s_Operations.push_back(new DetailsImageOperation());
	//s_Operations.push_back(new WaveletComposeImageOperation());
	s_Operations.push_back(new LutImageOperation());
}

FluxImage* FluxImageProcessor::DebugProcessToLuma(FluxImage* image)
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


	CropImageOperation crop = CropImageOperation();

	ProcessingCacheEntry cache = ProcessingCacheEntry();
	cache.CachesCount = 1;

	InternalImageData internalImage = InternalImageData();
	internalImage.RPixels = rChannel;
	internalImage.GPixels = gChannel;
	internalImage.BPixels = bChannel;
	internalImage.Width = width;
	internalImage.Height = height;

	cache.Caches = &internalImage;

	ProcessingCacheEntry cropCache = ProcessingCacheEntry();

	auto cropTime = BenchmarkHelper::StartWatch();

	ProcessingCacheEntry* newCropCache = crop.Run(&cache, &cropCache, &settings->Layers[0]);

	BenchmarkHelper::ShowDurationFinal(cropTime, "Crop took: ");

	cropCache = *newCropCache;

	rChannel = ((InternalImageData*)cropCache.Caches)->RPixels;
	gChannel = ((InternalImageData*)cropCache.Caches)->GPixels;
	bChannel = ((InternalImageData*)cropCache.Caches)->BPixels;

	width = ((InternalImageData*)cropCache.Caches)->Width;
	height = ((InternalImageData*)cropCache.Caches)->Height;

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

	ColorLUTConverter::ConvertToLab(rMat.GetPointer(), gMat.GetPointer(), bMat.GetPointer(), rMat.GetPointer(), gMat.GetPointer(), bMat.GetPointer(), rMat.Width(), rMat.Height());

	//Color manage 
	//I don't have time to handle any camera out there
	std::string outputFilePath2 = "C:\\Users\\Artyom\\Downloads\\Lena-B.jpg";
	JpegImageEncoder encoder1 = JpegImageEncoder((pixel_t*)bMat.GetPointer(), rMat.Width(), rMat.Height(), 1);
	encoder1.Init();
	encoder1.FastSave(outputFilePath2);

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
	firstLayer.CachesCount = 7;
	firstLayer.Caches = new ProcessingCacheEntry[7]{};
	ProcessingCacheEntry& firstEntry = firstLayer.Caches[0];
	firstEntry.Stage = ProcessingStage::Input;
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
	for (int i = cache->LayersCount - 1; i > changedLayer; i--)
	{
		ProcessingLayerCache& layer = cache->Layers[i];
		DisposeLayer(&layer);
	}


	ProcessingLayerCache& currentLayer = cache->Layers[settings->ChangedLayer];

	//We intend that order of the caches are increasing
	//Order is from first to last correction
	//So we get rid of everything we don't need until we encounter our cache that might be useful
	for (int i = currentLayer.CachesCount - 1; i > correctionStage; i--)
	{
		DisposeStageCache(&currentLayer.Caches[i]);
	}

	ProcessSettingsLayer currentLayerSettings = settings->Layers[settings->ChangedLayer];

	//int correctionPipelineLength = 1;
	for (int i = correctionStage; i < s_Operations.size(); i++)
	{
		BaseImageOperation* op = s_Operations[i];
		ProcessingCacheEntry* newCache = op->Run(&currentLayer.Caches[std::max(i - 1, 0)], &currentLayer.Caches[i], &currentLayerSettings);
		//No need to delete old cache because operation handles that for me 
		currentLayer.Caches[i] = *newCache;
	}

	//Must be Internal image data
	InternalImageData* lastCache = (InternalImageData*)currentLayer.Caches[s_Operations.size() - 1].Caches;

	pixel_t* rgbOut = new pixel_t[lastCache->Width * lastCache->Height * 3];

	PixelsHelper::Interleave3(lastCache->RPixels, lastCache->GPixels, lastCache->BPixels, rgbOut, lastCache->Width, lastCache->Height);

	FluxImage* result = new FluxImage();

	result->ChannelsCount = 3;
	result->Width = lastCache->Width;
	result->Height = lastCache->Height;
	result->Pixels = rgbOut;

	//When we got all we need

	return result;
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
		/*case ProcessingStage::NotSet:
			return;

		case ProcessingStage::Initial:
			return;

		case ProcessingStage::Resize: {
			InternalLabImage* imageResized = (InternalLabImage*)entry->Caches;
			delete[] imageResized->LPixels;
			delete[] imageResized->APixels;
			delete[] imageResized->BPixels;
			delete imageResized;
			return;
		}

		case ProcessingStage::Rotate:
		{
			InternalLabImage* imageRotated = (InternalLabImage*)entry->Caches;
			delete[] imageRotated->LPixels;
			delete[] imageRotated->APixels;
			delete[] imageRotated->BPixels;
			delete imageRotated;
			return;
		}*/


	case ProcessingStage::Crop: {
		InternalLabImage* imageCropped = (InternalLabImage*)entry->Caches;
		delete[] imageCropped->LPixels;
		delete[] imageCropped->APixels;
		delete[] imageCropped->BPixels;
		delete imageCropped;
		return;
	}


	case ProcessingStage::Denoise: {
		WaveletCache* cache = (WaveletCache*)entry->Caches;
		for (int i = 0; i < cache->WaveletDecCount; i++)
		{
			cache->LDec->at(i).Dispose();
			cache->ADec->at(i).Dispose();
			cache->BDec->at(i).Dispose();

		}
		cache->LDec->clear();
		cache->ADec->clear();
		cache->BDec->clear();

		delete cache->LDec;
		delete cache->ADec;
		delete cache->BDec;

		delete cache;
		return;
	}


								 /*case ProcessingStage::Dehaze: {
									 InternalLabImage* imageDehazed = (InternalLabImage*)entry->Caches;
									 delete[] imageDehazed->LPixels;
									 delete[] imageDehazed->APixels;
									 delete[] imageDehazed->BPixels;
									 delete imageDehazed;
									 return;
								 }*/

	case ProcessingStage::Details: {
		WaveletCache* cache = (WaveletCache*)entry->Caches;
		for (int i = 0; i < cache->WaveletDecCount; i++)
		{
			cache->LDec->at(i).Dispose();
			cache->ADec->at(i).Dispose();
			cache->BDec->at(i).Dispose();

		}
		cache->LDec->clear();
		cache->ADec->clear();
		cache->BDec->clear();

		delete cache->LDec;
		delete cache->ADec;
		delete cache->BDec;

		delete cache;
		return;
	}


								 /*case ProcessingStage::Texture: {
									 WaveletCache* cache = (WaveletCache*)entry->Caches;
									 for (int i = 0; i < cache->WaveletDecCount; i++)
									 {
										 cache->LDec->at(i).Dispose();
										 cache->ADec->at(i).Dispose();
										 cache->BDec->at(i).Dispose();

									 }
									 cache->LDec->clear();
									 cache->ADec->clear();
									 cache->BDec->clear();

									 delete cache->LDec;
									 delete cache->ADec;
									 delete cache->BDec;

									 delete cache;
									 return;
								 }*/


	case ProcessingStage::Lut: {
		InternalLabImage* imageColorCorrected = (InternalLabImage*)entry->Caches;
		delete[] imageColorCorrected->LPixels;
		delete[] imageColorCorrected->APixels;
		delete[] imageColorCorrected->BPixels;
		delete imageColorCorrected;
		return;
	}

	case ProcessingStage::WaveletDecompose: {
		WaveletCache* cache = (WaveletCache*)entry->Caches;
		for (int i = 0; i < cache->WaveletDecCount; i++)
		{
			cache->LDec->at(i).Dispose();
			cache->ADec->at(i).Dispose();
			cache->BDec->at(i).Dispose();

		}
		cache->LDec->clear();
		cache->ADec->clear();
		cache->BDec->clear();

		delete cache->LDec;
		delete cache->ADec;
		delete cache->BDec;

		delete cache;
		return;
	}

	case ProcessingStage::WaveletCompose: {
		InternalLabImage* imageColorCorrected = (InternalLabImage*)entry->Caches;
		delete[] imageColorCorrected->LPixels;
		delete[] imageColorCorrected->APixels;
		delete[] imageColorCorrected->BPixels;
		delete imageColorCorrected;
		return;
	}

	default:
		return;
	}

}
