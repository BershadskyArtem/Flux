#pragma once
#include <cmath>
#include "xsimd/xsimd.hpp"
#include "../../../infrastructure/FLXDefines.h"
#include "../BaseImageOperation.h"


class CropImageOperation : BaseImageOperation {
public:
	// Inherited via BaseImageOperation
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;

	// Inherited via BaseImageOperation
	virtual void Dispose() override;
};