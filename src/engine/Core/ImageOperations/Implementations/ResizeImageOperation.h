#pragma once
#include "../../../infrastructure/FLXDefines.h"
#include "../../../infrastructure/InteropTypes.h"
#include "../BaseImageOperation.h"
#include "../../Filters/BoxBlur.h"

class ResizeImageOperation : public BaseImageOperation {
protected:
	void Subsample(Matrix<pixel_t> &input, Matrix<pixel_t> &output, pixel_t subsamplingX, pixel_t subsamplingY);


public:
	// Inherited via BaseImageOperation
	virtual ProcessingCacheEntry* Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings) override;
	virtual void Dispose() override;
};