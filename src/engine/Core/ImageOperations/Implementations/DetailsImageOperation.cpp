#include "DetailsImageOperation.h"

//1.6, 1.3, 1.0, 1.0
//1.0f ,1.0f ,1.225f ,1.125f
//std::vector<pixel_t> DetailsImageOperation::s_SharpenMultipliers = { 1.6f ,1.3f ,1.0f ,1.0f, 1.0f ,1.0f , 1.0f ,1.0f , 1.0f ,1.0f , 1.0f ,1.0f };
std::vector<pixel_t> DetailsImageOperation::s_SharpenMultipliers = { 1.01f ,1.3f ,1.3f ,1.0f, 1.0f ,1.0f , 1.0f ,1.0f , 1.0f ,1.0f , 1.0f ,1.0f };
//std::vector<pixel_t> DetailsImageOperation::s_TextureMultipliers = { 1.0f ,1.0f ,1.225f ,1.125f, 1.0f ,1.0f, 1.0f ,1.0f, 1.0f ,1.0f };
std::vector<pixel_t> DetailsImageOperation::s_TextureMultipliers = { 1.0f ,1.0f ,1.0 ,1.125f, 1.05f ,1.0f, 1.0f ,1.0f, 1.0f ,1.0f };

WaveletImage<pixel_t>* DetailsImageOperation::ApplyToLevel(WaveletImage<pixel_t>& image, pixel_t value)
{
	int w = image.Width;
	int h = image.Height;

	int inc = vfloat::size;

	vfloat multiplierV = value;

	pixel_t multiplier = value;

	Matrix<pixel_t>* cdMat = image.CD->Copy();
	Matrix<pixel_t>* cvMat = image.CV->Copy();
	Matrix<pixel_t>* chMat = image.CH->Copy();

	//Copy wavelet level
	WaveletImage<pixel_t>* result = new WaveletImage<pixel_t>();
	result->Width = image.Width;
	result->InitialWidth = image.InitialWidth;
	result->Height = image.Height;
	result->InitialHeight = image.InitialHeight;

	result->CA = image.CA->Copy();
	result->CD = cdMat;
	result->CV = cvMat;
	result->CH = chMat;

	pixel_t* cd = cdMat->GetPointer();
	pixel_t* ch = cdMat->GetPointer();
	pixel_t* cv = cdMat->GetPointer();

	//No need to trash the caches

#pragma omp parallel for
	for (int y = 0; y < h; y++)
	{
		int startOfLine = y * w;
		int x = 0;
		for (; x < w - inc; x += inc)
		{
			vfloat line = vfloat::load_aligned(&cd[startOfLine + x]);
			line *= multiplierV;
			line.store_aligned(&cd[startOfLine + x]);
		}


		for (; x < w; x++)
		{
			cd[startOfLine + x] *= multiplier;
		}
	}


#pragma omp parallel for
	for (int y = 0; y < h; y++)
	{
		int startOfLine = y * w;
		int x = 0;
		for (; x < w - inc; x += inc)
		{
			vfloat line = vfloat::load_aligned(&cv[startOfLine + x]);
			line *= multiplierV;
			line.store_aligned(&cv[startOfLine + x]);
		}


		for (; x < w; x++)
		{
			cv[startOfLine + x] *= multiplier;
		}
	}

#pragma omp parallel for
	for (int y = 0; y < h; y++)
	{
		int startOfLine = y * w;
		int x = 0;
		for (; x < w - inc; x += inc)
		{
			vfloat line = vfloat::load_aligned(&ch[startOfLine + x]);
			line *= multiplierV;
			line.store_aligned(&ch[startOfLine + x]);
		}


		for (; x < w; x++)
		{
			ch[startOfLine + x] *= multiplier;
		}
	}

	return result;
}

ProcessingCacheEntry* DetailsImageOperation::Run(ProcessingCacheEntry* previousCachedStage, ProcessingCacheEntry* currentCachedStage, ProcessSettingsLayer* newSettings)
{
	pixel_t sharpen = newSettings->Clarity.Amount / 100.f;
	pixel_t texture = newSettings->Texture.Amount / 100.f;
	sharpen -= 1;
	texture -= 1;

	//Current cache is useless
	WaveletCache* cacheToDelete = (WaveletCache*)currentCachedStage->Caches;

	if (cacheToDelete != nullptr) {
		for (int i = 0; i < cacheToDelete->LDec->size(); i++)
		{
			cacheToDelete->LDec->at(i).Dispose();
		}
		cacheToDelete->LDec->clear();

		for (int i = 0; i < cacheToDelete->ADec->size(); i++)
		{
			cacheToDelete->ADec->at(i).Dispose();
		}
		cacheToDelete->ADec->clear();


		for (int i = 0; i < cacheToDelete->BDec->size(); i++)
		{
			cacheToDelete->BDec->at(i).Dispose();
		}
		cacheToDelete->BDec->clear();

		delete cacheToDelete->LDec;
		delete cacheToDelete->ADec;
		delete cacheToDelete->BDec;

		delete cacheToDelete;
	}

	//Use previous cache
	WaveletCache* cache = (WaveletCache*)previousCachedStage->Caches;

	//Now we need to apply multipiers to wavelet coefficients

	int depthToApply = std::min(cache->LDec->size(), s_TextureMultipliers.size());

	WaveletCache* newCache = new WaveletCache();
	newCache->LDec = new std::vector<WaveletImage<pixel_t>>();
	newCache->LDec->reserve(depthToApply);

	newCache->ADec = new std::vector<WaveletImage<pixel_t>>();
	newCache->ADec->reserve(depthToApply);

	newCache->BDec = new std::vector<WaveletImage<pixel_t>>();
	newCache->BDec->reserve(depthToApply);

	//Apply texture and sharpen
	for (int i = 0; i < depthToApply; i++)
	{
		//pixel_t multiplier = sharpen * s_SharpenMultipliers[i] + texture * s_TextureMultipliers[i];

		//pixel_t multiplier = s_SharpenMultipliers[i] * sharpen + s_TextureMultipliers[i] * texture;
		// 
		//pixel_t multiplier = FluxMath::LinearInterpolate(sharpen, s_SharpenMultipliers[i], 1.0f);
		pixel_t multiplier = FluxMath::LinearInterpolate(texture, s_TextureMultipliers[i], 1.0f);
		multiplier *= FluxMath::LinearInterpolate(sharpen, s_SharpenMultipliers[i], 1.0f);
		WaveletImage<pixel_t>* enhancedL = ApplyToLevel(cache->LDec->at(i), multiplier);
		newCache->LDec->push_back(*enhancedL);
	}

	//Copy other channels
	for (int i = 0; i < depthToApply; i++)
	{
		//Copy A wavelet image
		WaveletImage<pixel_t> levelA = cache->ADec->at(i).Copy();
		newCache->ADec->push_back(levelA);
	}

	for (int i = 0; i < depthToApply; i++)
	{
		//Copy A wavelet image
		WaveletImage<pixel_t> levelB = cache->BDec->at(i).Copy();
		newCache->BDec->push_back(levelB);
	}

	currentCachedStage->Caches = newCache;

	return currentCachedStage;
}

void DetailsImageOperation::Dispose()
{
}
