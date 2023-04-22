#pragma once
#include "../infrastructure/FLXDefines.h"


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

struct WaveletImage {
    pixel_t* LL;
    pixel_t* HL;
    pixel_t* LH;
    pixel_t* HH;
    int Width;
    int Height;
    inline void Dispose() {
        if (LL != nullptr)
            delete[] LL;
        if (HL != nullptr)
            delete[] HL;
        if (LH != nullptr)
            delete[] LH;
        if (HH != nullptr)
            delete[] HH;
    }
};