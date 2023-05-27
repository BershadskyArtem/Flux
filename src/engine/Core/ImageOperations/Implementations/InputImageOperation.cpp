#include "InputImageOperation.h"

ProcessingCacheEntry* InputImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	return currentCachedStage;
}

void InputImageOperation::Dispose()
{
}
