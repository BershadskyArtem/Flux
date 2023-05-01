#pragma once
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/Matrix.h"


struct WaveletLine {
public:
    WaveletLine(pixel_t* hi, pixel_t* lo, int length);
    
	pixel_t* Hi;
	pixel_t* Lo;
	int Length;
	inline void Dispose() {
		if (Hi != nullptr)
			delete[] Hi;
		if (Lo != nullptr)
			delete[] Lo;
	}
};

class WaveletData {
public:
    WaveletData(int size, pixel_t* lowPassDeconstruction, pixel_t* highPassDeconstruction, pixel_t* lowPassReconstruction, pixel_t* highPassReconstruction)
    {
        Size = size;
        LowPassDeconstruction = lowPassDeconstruction;
        HighPassDeconstruction = highPassDeconstruction;
        LowPassReconstruction = lowPassReconstruction;
        HighPassReconstruction = highPassReconstruction;
    }

    int Size;
    pixel_t* LowPassDeconstruction;
    pixel_t* HighPassDeconstruction;
    pixel_t* LowPassReconstruction;
    pixel_t* HighPassReconstruction;
    inline void Dispose() {
        if (LowPassDeconstruction != nullptr)
            delete[] LowPassDeconstruction;
        if (HighPassDeconstruction != nullptr)
            delete[] HighPassDeconstruction;
        if (LowPassReconstruction != nullptr)
            delete[] LowPassReconstruction;
        if (HighPassReconstruction != nullptr)
            delete[] HighPassReconstruction;
    }

    ~WaveletData();
};

template <typename T>
struct WaveletImage {
    Matrix<T>* CH;
    Matrix<T>* CD;
    Matrix<T>* CA;
    Matrix<T>* CV;
    WaveletImage();
    int Width;
    int Height;
    int InitialWidth; 
    int InitialHeight;
    inline WaveletImage Copy() {
        WaveletImage<T> result = WaveletImage<T>();
        result.CA = CA->Copy();
        result.CD = CD->Copy();
        result.CH = CH->Copy();
        result.CV = CV->Copy();
        result.Width = Width;
        result.Height = Height;
        result.InitialHeight = InitialHeight;
        result.InitialWidth = InitialWidth;
        return result;
    }

    inline void Dispose() {
        CH->Dispose();
        CD->Dispose();
        CA->Dispose();
        CV->Dispose();
    }
};


