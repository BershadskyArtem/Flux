#include "InputImageOperation.h"

ProcessingCacheEntry* InputImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	return currentCachedStage;
}

void InputImageOperation::Dispose()
{
}

void InputImageOperation::DisposeCacheEntry(ProcessingCacheEntry* cache)
{
	InternalImageData* image = (InternalImageData*)cache->Caches;

	if (image == nullptr)
		return;
	
	if (image->RPixels != nullptr)
		delete[] image->RPixels;
	if (image->GPixels != nullptr)
		delete[] image->GPixels;
	if (image->BPixels != nullptr)
		delete[] image->BPixels;

	delete image;
	cache->Caches = nullptr;
	cache->CachesCount = 0;
}
