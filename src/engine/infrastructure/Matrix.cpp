#include "Matrix.h"

template<typename T>
Matrix<T>::Matrix(int width, int height)
{
	this->m_Width = width;
	this->m_Height = height;
	this->m_Values = new T[width * height]{};
}
template<typename T>
Matrix<T>::Matrix(int width, int height, T* value)
{
	this->m_Width = width;
	this->m_Height = height;
	this->m_Values = value;
}

template<typename T>
Matrix<T>::~Matrix()
{

}

template<typename T>
T* Matrix<T>::GetPointer(int start)
{
	return &m_Values[start];
}

template<typename T>
T* Matrix<T>::GetPointer(int idx, int idy)
{
	return GetPointer(idy * Width() + idx);
}

template<typename T>
std::vector<T> Matrix<T>::GetRow(int idy)
{
	std::vector<T> result = std::vector<T>();
	result.reserve(Width());
	for (int x = 0; x < Width(); x++)
	{
		T value = this->at(x, idy);
		result.push_back(value);
	}
	return result;
}

template<typename T>
std::vector<T> Matrix<T>::GetColumn(int idx)
{
	
	std::vector<T> result = std::vector<T>();
	result.reserve(Height());
	for (int x = 0; x < Height(); x++)
	{
		T value = this->at(idx, x);
		result.push_back(value);
	}
	return result;
}

template<typename T>
void Matrix<T>::SetRow(int idy, T* buff)
{
	T* ptr = GetPointer(0, idy);
	for (int x = 0; x < Width(); x++)
	{
		ptr[x] = buff[x];
	}
}

template<typename T>
void Matrix<T>::SetColumn(int idx, T* buff)
{
	for (int y = 0; y < Height(); y++)
	{
		int index = y * Width() + idx;
		m_Values[index] = buff[y];
	}
}

template<typename T>
void Matrix<T>::Print()
{
	if (m_Values == nullptr) {
		std::cout << "Disposed" << '\n';
		return;
	}


	for (int i = 0; i < Height(); i++)
	{
		for (int j = 0; j < Width(); j++)
		{
			std::cout << at(j, i) << '\t';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
	std::cout << '\n';
}

template<typename T>
void Matrix<T>::Dispose()
{
	if (m_Values != nullptr) {
		delete[] m_Values;
		m_Values = nullptr;
	}
}

template<typename T>
int Matrix<T>::Width()
{
	return m_Width;
}

template<typename T>
int Matrix<T>::Height()
{
	return m_Height;
}



template<typename T>
T& Matrix<T>::at(int idx)
{
	return m_Values[idx];
}

template<typename T>
T& Matrix<T>::at(int idx, int idy)
{
	int index = idy * m_Width + idx;
	return m_Values[index];
}


template class Matrix<pixel_t>;