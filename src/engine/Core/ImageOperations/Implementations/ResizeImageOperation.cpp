#include "ResizeImageOperation.h"

void ResizeImageOperation::Subsample(Matrix<pixel_t>& input, Matrix<pixel_t>& output, pixel_t subsamplingX, pixel_t subsamplingY)
{
	const int oldWidth = input.Width();
	const int oldHeight = input.Height();

	const int newWidth = output.Width();
	const int newHeight = output.Height();

	const pixel_t* inputPixels = input.GetPointer();
	pixel_t* outputPixels = output.GetPointer();

	for (int y = 0; y < newHeight; y++)
	{
		int newStartOfLine = y * newWidth;
		const int yPrev = std::floor(y / subsamplingY);
		
		for (int x = 0; x < newWidth; x++)
		{
			const int idx = newStartOfLine + x;
			const int xPrev = std::floor(x / subsamplingX);
			outputPixels[idx] = inputPixels[yPrev * oldWidth + xPrev];
		}
	}


	

}

ProcessingCacheEntry* ResizeImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	//We need to dispose old cache 
	if (currentCachedStage->Caches != nullptr) {
		auto cacheToDelete = (InternalLabImage*)currentCachedStage->Caches;
		if (cacheToDelete->LPixels != nullptr)
			delete[] cacheToDelete->LPixels;
		if (cacheToDelete->APixels != nullptr)
			delete[] cacheToDelete->APixels;
		if (cacheToDelete->BPixels != nullptr)
			delete[] cacheToDelete->BPixels;
		delete cacheToDelete;
	}


	InternalLabImage* previousStageCache = (InternalLabImage*)previousCachedStage->Caches;
	const int oldWidth = previousStageCache->Width;
	const int oldHeight = previousStageCache->Height;

	int newWidth = newSettings->Resize.ResizeToX;
	int newHeight = newSettings->Resize.ResizeToY;

	const bool isNumericScaleMode = newSettings->Resize.Mode == 1;

	pixel_t scaleX = 1;
	pixel_t scaleY = 1;

	if (isNumericScaleMode) {
		scaleX = newSettings->Resize.Scale;
		scaleY = newSettings->Resize.Scale;
		newWidth = oldWidth * scaleX;
		newHeight = oldHeight * scaleY;
	}
	else {
		scaleX = static_cast<pixel_t>(oldWidth) / newWidth;
		scaleY = static_cast<pixel_t>(oldHeight) / newHeight;
	}


	InternalLabImage* newCache = new InternalLabImage();
	newCache->Width = newWidth;
	newCache->Height = newHeight;
	
	const int boxWidth = std::ceil(1 / scaleX);
	const int boxHeight = std::ceil(1 / scaleY);

	//Box downscaling is just a box blur with subsampling

	const int boxBlurRadius = std::ceil(std::max(boxWidth, boxHeight) / 2.0);

	pixel_t* buffer = new pixel_t[oldWidth * oldHeight];
	Matrix<pixel_t> bufferMat = Matrix<pixel_t>(oldWidth, oldHeight, buffer);
	Matrix<pixel_t> outputL = Matrix<pixel_t>(newWidth, newHeight, new pixel_t[newWidth * newHeight]);
	Matrix<pixel_t> outputA = Matrix<pixel_t>(newWidth, newHeight, new pixel_t[newWidth * newHeight]);
	Matrix<pixel_t> outputB = Matrix<pixel_t>(newWidth, newHeight, new pixel_t[newWidth * newHeight]);

	//Resize L
	Matrix<pixel_t> inputL = Matrix<pixel_t>(oldWidth, oldHeight, previousStageCache->LPixels);
	BoxBlur::Blur(inputL, bufferMat, boxBlurRadius);
	Subsample(bufferMat, outputL, 1.0 / scaleX, 1.0 / scaleY);

	//Resize A
	Matrix<pixel_t> inputA = Matrix<pixel_t>(oldWidth, oldHeight, previousStageCache->APixels);
	BoxBlur::Blur(inputA, bufferMat, boxBlurRadius);
	Subsample(bufferMat, outputA, 1.0 / scaleX, 1.0 / scaleY);

	//Resize B
	Matrix<pixel_t> inputB = Matrix<pixel_t>(oldWidth, oldHeight, previousStageCache->BPixels);
	BoxBlur::Blur(inputB, bufferMat, boxBlurRadius);
	Subsample(bufferMat, outputB, 1.0 / scaleX, 1.0 / scaleY);

	delete[] buffer;

	newCache->LPixels = outputL.GetPointer();
	newCache->APixels = outputA.GetPointer();
	newCache->BPixels = outputB.GetPointer();

	currentCachedStage->Caches = newCache;
	currentCachedStage->CachesCount = 1;

	/*for (int y = 0; y < newHeight; y++)
	{
		for (int x = 0; x < newWidth; x++)
		{
			const int oldXStart = std::floor(x / scaleX);
			const int oldYStart = std::floor(x / scaleY);
			
			const int oldXEnd = std::min(oldXStart + boxWidth, oldWidth - 1);
			const int oldYEnd = std::min(oldYStart + boxHeight, oldHeight - 1);




		}
	}*/



	return currentCachedStage;
}

void ResizeImageOperation::Dispose()
{
}
