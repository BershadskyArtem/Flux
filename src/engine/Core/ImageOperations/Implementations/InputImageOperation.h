#pragma once
#include "../BaseImageOperation.h"
#include "../../../infrastructure/FLXDefines.h"
#include "../../../infrastructure/PixelsHelper.h"

class InputImageOperation : public BaseImageOperation {
	// Inherited via BaseImageOperation
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;

	// Inherited via BaseImageOperation
	virtual void DisposeCacheEntry(ProcessingCacheEntry* cache) override;
};