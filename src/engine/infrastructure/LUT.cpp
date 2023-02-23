#include "LUT.h"
//#include "MathHelpers.h"
#include <cmath>


void LUT::Init(int size, pixel_t defaultValue)
{
	sizeV = static_cast<float>(size);
	sizeWithPadding = size + 32;
	oneV = 1;
	this->size = size;
	data = new pixel_t[sizeWithPadding];

	for (int i = 0; i < sizeWithPadding; i++)
	{
		data[i] = defaultValue;
	}
}

void LUT::Dispose()
{
	if (data == nullptr)
		return;
	delete[] data;
	data = nullptr;
}

void LUT::SetValue(pixel_t value, int index)
{
	data[index] = value;
}

pixel_t LUT::Get(pixel_t value)
{
	//int index = static_cast<int>(std::ceilf(value));
	//int indexLow = static_cast<int>(std::floorf(value));

	int index = (int)value;
	float diff = value - (float)index;
	auto p1 = data[index];
	auto p2 = data[index + 1] - p1;
	return p1 + p2 * diff;
}

pixel_t LUT::Get(int value)
{
	if (value >= this->size)
		return data[size - 1];
	return data[value];
}

pixel_t LUT::Get01(pixel_t value)
{
	value *= this->size;
	int index = (int)value;
	float diff = value - (float)index;
	auto p1 = data[index];
	auto p2 = data[index + 1] - p1;
	return p1 + p2 * diff;
}
