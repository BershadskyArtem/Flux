#include "ColorLUTConverter.h"



LUTf3d ColorLUTConverter::s_LabLutL = LUTf3d();
LUTf3d ColorLUTConverter::s_LabLutA = LUTf3d();
LUTf3d ColorLUTConverter::s_LabLutB = LUTf3d();
bool ColorLUTConverter::s_IsInitialized = false;


bool ColorLUTConverter::IsInitialized() {
	return s_IsInitialized;
}

void ColorLUTConverter::InitLabLut()
{
	s_LabLutL.Init(LUT_DIM, LUT_DIM, LUT_DIM);
	s_LabLutA.Init(LUT_DIM, LUT_DIM, LUT_DIM);
	s_LabLutB.Init(LUT_DIM, LUT_DIM, LUT_DIM);

	for (int r = 0; r < LUT_DIM; r++)
	{
		for (int g = 0; g < LUT_DIM; g++)
		{
			for (int b = 0; b < LUT_DIM; b++)
			{
				pixel_t rValue = r / (float)LUT_DIM;
				pixel_t gValue = g / (float)LUT_DIM;
				pixel_t bValue = b / (float)LUT_DIM;

				Converter::RGB2OKLab(rValue, gValue, bValue, rValue, gValue, bValue);

				s_LabLutL.SetValue(rValue, r, g, b);
				s_LabLutA.SetValue(gValue, r, g, b);
				s_LabLutB.SetValue(bValue, r, g, b);
			}
		}
	}
}

void ColorLUTConverter::Init()
{
	if (s_IsInitialized)
		return;

	Converter::Init();
	InitLabLut();
	s_IsInitialized = true;
}

void ColorLUTConverter::ConvertToLab(pixel_t* rIn, pixel_t* gIn, pixel_t* bIn, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height)
{
	int size = width * height;
	int inc = vfloat::size;
	//#pragma omp parallel for
	for (int x = 0; x < size - vfloat::size; x += inc)
	{
		if (x == 19419551) {
			int dk = 100;
		}

		vfloat rV = vfloat::load_aligned(&rIn[x]);
		vfloat gV = vfloat::load_aligned(&gIn[x]);
		vfloat bV = vfloat::load_aligned(&bIn[x]);


		vfloat lV = s_LabLutL.Get01(rV, gV, bV);
		vfloat aV = s_LabLutA.Get01(rV, gV, bV);
		//Here we dont need to create new vfloat, we can use the old one
		bV = s_LabLutB.Get01(rV, gV, bV);

		lV.store_aligned(&lOut[x]);
		aV.store_aligned(&aOut[x]);
		bV.store_aligned(&bOut[x]);
	}

	for (int x = size - vfloat::size; x < size; x++)
	{
		float r = rIn[x];
		float g = gIn[x];
		float b = bIn[x];


		float l = s_LabLutL.Get01(r, g, b);
		float a = s_LabLutA.Get01(r, g, b);
		b = s_LabLutA.Get01(r, g, b);

		lOut[x] = l;
		aOut[x] = a;
		bOut[x] = b;
	}
}
