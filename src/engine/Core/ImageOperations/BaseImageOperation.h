#pragma once
#include "../../infrastructure/InteropTypes.h"


class BaseImageOperation {
public:
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) = 0;
	virtual void Dispose() = 0;
};