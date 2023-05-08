#pragma once
#include "../../infrastructure/InteropTypes.h"


class BaseImageOperation {
public:
	ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage) = 0;
};