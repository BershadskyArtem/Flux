#include "LUTf3d.h"

void LUTf3d::Init(int sizeX, int sizeY, int sizeZ, pixel_t defaultValue)
{
	int size = sizeX * sizeY * sizeZ;

	this->depth = sizeZ;
	this->width = sizeX;
	this->height = sizeY;

	sizeV = static_cast<float>(size);
	sizeWithPadding = size + 32;
	oneV = 1;
	this->size = size;
	data = new pixel_t[sizeWithPadding];
#pragma omp parallel for
	for (int i = 0; i < sizeWithPadding; i++)
	{
		data[i] = defaultValue;
	}

	widthVi = sizeX;
	heightVi = sizeY;
	depthVi = sizeZ;

	widthV = static_cast<float>(sizeX);
	heightV = static_cast<float>(sizeY);
	depthV = static_cast<float>(sizeZ);
	oneVf = 1.f;
	zeroV = 0;
}

void LUTf3d::Dispose()
{
	if (data == nullptr)
		return;
	delete[] data;
	data = nullptr;
}

void LUTf3d::SetValue(pixel_t value, int x, int y, int z)
{
	int idx = GetIndex(x, y, z);
	data[idx] = value;
}

pixel_t LUTf3d::Get(pixel_t valueX, pixel_t valueY, pixel_t valueZ)
{
	int idxX = (int)valueX;
	float diffX = valueX - (float)idxX;
	int idxY = (int)valueY;
	float diffY = valueY - (float)idxY;
	int idxZ = (int)valueZ;
	float diffZ = valueZ - (float)idxZ;
	
	int idxXOne = std::min(this->width - 1, idxX + 1);
	int idxYOne = std::min(this->height - 1, idxY + 1);
	int idxZOne = std::min(this->depth - 1, idxZ + 1);

	pixel_t& c000 = data[GetIndex(idxX, idxY, idxZ)];
	pixel_t& c100 = data[GetIndex(idxXOne, idxY, idxZ)];
	pixel_t& c010 = data[GetIndex(idxX, idxYOne, idxZ)];
	pixel_t& c110 = data[GetIndex(idxXOne, idxYOne, idxZ)];
	pixel_t& c001 = data[GetIndex(idxX, idxY, idxZOne)];
	pixel_t& c101 = data[GetIndex(idxXOne, idxY, idxZOne)];
	pixel_t& c011 = data[GetIndex(idxX, idxYOne, idxZOne)];
	pixel_t& c111 = data[GetIndex(idxXOne, idxYOne, idxZOne)];

	return
		(1 - diffX) * (1 - diffY) * (1 - diffZ) * c000 +
		diffX * (1 - diffY) * (1 - diffZ) * c100 +
		(1 - diffX) * diffY * (1 - diffZ) * c010 +
		diffX * diffY * (1 - diffZ) * c110 +
		(1 - diffX) * (1 - diffY) * diffZ * c001 +
		diffX * (1 - diffY) * diffZ * c101 +
		(1 - diffX) * diffY * diffZ * c011 +
		diffX * diffY * diffZ * c111;
}

pixel_t LUTf3d::Get(int x, int y, int z)
{
	return data[GetIndex(x, y, z)];
}

pixel_t LUTf3d::Get01(pixel_t valueX, pixel_t valueY, pixel_t valueZ)
{
	valueX = std::fminf(this->width - 1, valueX * (this->width - 1));
	valueY = std::fminf(this->height - 1, valueY * (this->height - 1));
	valueZ = std::fminf(this->depth - 1, valueZ * (this->depth - 1));

	return Get(valueX, valueY, valueZ);
}

void LUTf3d::ApplyLUTs(pixel_t* rIn, pixel_t* gIn, pixel_t* bIn, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, LUTf3d& lutR, LUTf3d& lutG, LUTf3d& lutB, int width, int height)
{
	int inc = vfloat::size;

#pragma omp parallel for
	for (int y = 0; y < height; y++)
	{
		int startOfLine = y * width;
		for (int x = 0; x < width - inc; x+=inc)
		{
			int index = startOfLine + x;
			vfloat rP = vfloat::load_aligned(&rIn[index]);
			vfloat gP = vfloat::load_aligned(&gIn[index]);
			vfloat bP = vfloat::load_aligned(&bIn[index]);
			vfloat nr = lutR.Get01(rP, gP, bP);
			vfloat ng = lutG.Get01(rP, gP, bP);
			vfloat nb = lutB.Get01(rP, gP, bP);
			nr.store_aligned(&rOut[index]);
			ng.store_aligned(&gOut[index]);
			nb.store_aligned(&bOut[index]);
		}

		for (int x = width - inc; x < width; x++)
		{
			int index = startOfLine + x;
			float rP = rIn[index];
			float gP = gIn[index];
			float bP = bIn[index];
			float nr = lutR.Get01(rP, gP, bP);
			float ng = lutG.Get01(rP, gP, bP);
			float nb = lutB.Get01(rP, gP, bP);
			rOut[index] = nr;
			gOut[index] = ng;
			bOut[index] = nb;
		}
	}
}
