#pragma once
#include "../BaseImageOperation.h"

class LutImageOperation : BaseImageOperation {
	// Inherited via BaseImageOperation
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;
};