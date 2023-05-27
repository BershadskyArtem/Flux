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
Matrix<T>::Matrix()
{
	m_Width = 0;
	m_Height = 0;
	m_Values = nullptr;
}


template<typename T>
Matrix<T>* Matrix<T>::Copy()
{
	T* values = new T[this->Width() * this->Height()];
	MemoryUtils::Copy((char*)m_Values, (char*)values, sizeof(T), this->Width() * this->Height());
	Matrix<T>* result = new Matrix<T>(this->Width(), this->Height(), values);
	return result;
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
std::vector<T> Matrix<T>::GetRow(int idy, bool evenSized)
{
	std::vector<T> result = std::vector<T>();

	int w = Width();

	if (w % 2 != 0 && evenSized)
		w += 1;

	result.reserve(w);
	for (int x = 0; x < Width(); x++)
	{
		T value = this->at(x, idy);
		result.push_back(value);
	}

	if (Width() % 2 != 0 && evenSized)
		result.push_back(result[result.size()-1]);

	return result;
}

template<typename T>
std::vector<T> Matrix<T>::GetColumn(int idx, bool evenSized)
{
	
	std::vector<T> result = std::vector<T>();

	int h = Height();

	if (h % 2 != 0 && evenSized)
		h += 1;

	result.reserve(h);
	for (int x = 0; x < Height(); x++)
	{
		T value = this->at(idx, x);
		result.push_back(value);
	}

	if (Height() % 2 != 0 && evenSized)
		result.push_back(result[result.size() - 1]);

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
int Matrix<T>::Width(bool forceEven)
{
	if (forceEven && m_Width % 2 != 0)
		return m_Width + 1;
	return m_Width;
}

template<typename T>
int Matrix<T>::Height(bool forceEven)
{
	if (forceEven && m_Height % 2 != 0)
		return m_Height + 1;
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