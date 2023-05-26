#include "CropImageOperation.h"

ProcessingCacheEntry* CropImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	//Lab/RGB image on input, image on entering

	//Delete current cache because we don't need it in this case
	if (currentCachedStage->CachesCount != 0) {
		InternalImageData* cacheToDelete = (InternalImageData*)currentCachedStage->Caches;
		if(cacheToDelete->RPixels != nullptr)
			delete[] cacheToDelete->RPixels;
		if (cacheToDelete->RPixels != nullptr)
			delete[] cacheToDelete->GPixels;
		if (cacheToDelete->RPixels != nullptr)
			delete[] cacheToDelete->BPixels;
		delete cacheToDelete;
		currentCachedStage->Caches = nullptr;
	}
	InternalImageData* image = (InternalImageData*) previousCachedStage->Caches;
	
	int width = image->Width;
	int height = image->Height;

	InternalImageData* newImage = new InternalImageData();

	CropProcessingSettings& settings = newSettings->Crop;

	int cropWidth = std::abs(settings.RightDownX - settings.LeftUpX);
	int cropHeight = std::abs(settings.RightDownY - settings.LeftUpY);

	newImage->Width = cropWidth;
	newImage->Height = cropHeight;

	newImage->RPixels = new pixel_t[cropHeight * cropWidth];
	newImage->GPixels = new pixel_t[cropHeight * cropWidth];
	newImage->BPixels = new pixel_t[cropHeight * cropWidth];

	int cropHeightIdy1 = settings.LeftUpY;
	int cropHeightIdy2 = settings.RightDownY;
	int cropWidthIdx1 = settings.LeftUpX;
	int cropWidthIdx2 = settings.RightDownX;

	int inc = vfloat::size;

//#pragma omp parallel for
	for (int y = cropHeightIdy1; y < cropHeightIdy2; y++)
	{
		int startOfLineWithoutCrop = y * width;
		int startOfLineWithCrop = (y - cropHeightIdy1) * cropWidth;
		int x = cropWidthIdx1;
		for (; x < cropWidthIdx2 - inc; x+=inc)
		{
			int idxWithoutCrop = startOfLineWithoutCrop + x;
			int idxWithCrop = startOfLineWithCrop + x - cropWidthIdx1;

			vfloat rPixels = vfloat::load_aligned(&image->RPixels[idxWithoutCrop]);
			vfloat gPixels = vfloat::load_aligned(&image->GPixels[idxWithoutCrop]);
			vfloat bPixels = vfloat::load_aligned(&image->BPixels[idxWithoutCrop]);

			rPixels.store_aligned(&newImage->RPixels[idxWithCrop]);
			gPixels.store_aligned(&newImage->GPixels[idxWithCrop]);
			bPixels.store_aligned(&newImage->BPixels[idxWithCrop]);
		}

		for (;x < cropWidthIdx2; x++)
		{
			int idxWithoutCrop = startOfLineWithoutCrop + x;
			int idxWithCrop = startOfLineWithCrop + x - cropWidthIdx1;

			pixel_t rPixel = image->RPixels[idxWithoutCrop];
			pixel_t gPixel = image->GPixels[idxWithoutCrop];
			pixel_t bPixel = image->BPixels[idxWithoutCrop];

			newImage->RPixels[idxWithCrop] = rPixel;
			newImage->GPixels[idxWithCrop] = gPixel;
			newImage->BPixels[idxWithCrop] = bPixel;
		}
	}

	currentCachedStage->CachesCount = 1;
	currentCachedStage->Caches = newImage;
	currentCachedStage->Stage = ProcessingStage::Crop;

	return currentCachedStage;
}

void CropImageOperation::Dispose()
{
}


//CropImageOperation u_Operation;