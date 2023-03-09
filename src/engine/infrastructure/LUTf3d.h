#pragma once
#include "FLXDefines.h";
#include <xsimd/xsimd.hpp>

class LUTf3d {
private:
	pixel_t* data;
	int size;
	int width;
	int height;
	int depth;
	int sizeWithPadding;
	vfloat sizeV;
	vfloat widthV;
	vfloat heightV;
	vfloat depthV;

	//vint sizeVi;
	vint widthVi;
	vint heightVi;
	vint depthVi;
	vint zeroV;

	vint oneV;
	vfloat oneVf;

	inline int GetIndex(int x, int y, int z) {
		return z * width * height + y * width + x;
	}

	inline vint GetIndex(vint x,
		vint y,
		vint z) {
		return z * width * height + y * width + x;
	}

public:
	void Init(int sizeX, int sizeY, int sizeZ, pixel_t defaultValue = 1.f);
	void Dispose();
	void SetValue(pixel_t value, int x, int y, int z);
	pixel_t Get(pixel_t valueX, pixel_t valueY, pixel_t valueZ);
	pixel_t Get(int x, int y, int z);

	pixel_t Get01(pixel_t valueX, pixel_t valueY, pixel_t valueZ);

	inline vfloat Get01(vfloat valueX, vfloat valueY, vfloat valueZ) {

		valueX = xsimd::min(this->widthV - oneVf, valueX * (this->widthV	- oneVf));
		valueY = xsimd::min(this->heightV - oneVf, valueY * (this->heightV	- oneVf));
		valueZ = xsimd::min(this->depthV - oneVf, valueZ * (this->depthV	- oneVf));

		vint idxX = xsimd::batch_cast<int>(valueX);
		vfloat diffX = valueX - xsimd::batch_cast<float>(idxX);
		vint idxY = xsimd::batch_cast<int>(valueY);
		vfloat diffY = valueY - xsimd::batch_cast<float>(idxY);
		vint idxZ = xsimd::batch_cast<int>(valueZ);
		vfloat diffZ = valueZ - xsimd::batch_cast<float>(idxZ);

		vint idxXOne = xsimd::min(this->widthVi - oneV, idxX + oneV);
		vint idxYOne = xsimd::min(this->heightVi - oneV, idxY + oneV);
		vint idxZOne = xsimd::min(this->depthVi - oneV, idxZ + oneV);

		vfloat c000 = vfloat::gather(data, GetIndex(idxX, idxY, idxZ));
		vfloat c100 = vfloat::gather(data, GetIndex(idxXOne, idxY, idxZ));
		vfloat c010 = vfloat::gather(data, GetIndex(idxX, idxYOne, idxZ));
		vfloat c110 = vfloat::gather(data, GetIndex(idxXOne, idxYOne, idxZ));
		vfloat c001 = vfloat::gather(data, GetIndex(idxX, idxY, idxZOne));
		vfloat c101 = vfloat::gather(data, GetIndex(idxXOne, idxY, idxZOne));
		vfloat c011 = vfloat::gather(data, GetIndex(idxX, idxYOne, idxZOne));
		vfloat c111 = vfloat::gather(data, GetIndex(idxXOne, idxYOne, idxZOne));

		return
			(oneVf - diffX) * (oneVf - diffY) * (oneVf - diffZ) * c000 +
			diffX * (oneVf - diffY) * (oneVf - diffZ) * c100 +
			(oneVf - diffX) * diffY * (oneVf - diffZ) * c010 +
			diffX * diffY * (oneVf - diffZ) * c110 +
			(oneVf - diffX) * (oneVf - diffY) * diffZ * c001 +
			diffX * (oneVf - diffY) * diffZ * c101 +
			(oneVf - diffX) * diffY * diffZ * c011 +
			diffX * diffY * diffZ * c111;
	}

	static void ApplyLUTs(pixel_t* rIn, pixel_t* gIn, pixel_t* bIn, pixel_t* rOut, pixel_t* gOut, pixel_t* bOut, LUTf3d& lutR, LUTf3d& lutG, LUTf3d& lutB, int width, int height);
	



};