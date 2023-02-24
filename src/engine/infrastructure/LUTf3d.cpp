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
	
	pixel_t& c000 = data[GetIndex(idxX, idxY, idxZ)];
	pixel_t& c100 = data[GetIndex(idxX + 1, idxY, idxZ)];
	pixel_t& c010 = data[GetIndex(idxX, idxY + 1, idxZ)];
	pixel_t& c110 = data[GetIndex(idxX + 1, idxY + 1, idxZ)];
	pixel_t& c001 = data[GetIndex(idxX, idxY, idxZ + 1)];
	pixel_t& c101 = data[GetIndex(idxX + 1, idxY, idxZ + 1)];
	pixel_t& c011 = data[GetIndex(idxX, idxY + 1, idxZ + 1)];
	pixel_t& c111 = data[GetIndex(idxX + 1, idxY + 1, idxZ + 1)];

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
