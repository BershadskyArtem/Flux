#pragma once
#include <vector>
#include <iostream>
#include "MemoryUtils.h"
template <typename T>
class Matrix {
private:
	T* m_Values;
	int m_Width;
	int m_Height;
public:
	Matrix(int width, int height);
	Matrix(int width, int height, T* value);
	~Matrix();
	T* GetPointer(int start = 0);
	T* GetPointer(int idx, int idy);
	std::vector<T> GetRow(int idy);
	std::vector<T> GetColumn(int idx);
	void SetRow(int idy, T* buff);
	void SetColumn(int idx, T* buff);
	void Print();
	void Dispose();
	int Width();
	int Height();
	T& at(int idx);
	T& at(int idx, int idy);
};