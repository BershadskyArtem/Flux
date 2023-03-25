#pragma once
#include "../infrastructure/FLXDefines.h"
#include "../infrastructure/LUTf3d.h"
#include "Converter.h"

class ColorLUTConverter {

private:
	static LUTf3d s_LabLutL;
	static LUTf3d s_LabLutA;
	static LUTf3d s_LabLutB;

	static void InitLabLut();

	static bool s_IsInitialized;

public:

	static bool IsInitialized();
		
	

	static void Init();
	/// <summary>
	/// This will be called one time on import. After that the image will be stored as Lab
	/// </summary>
	/// <param name="rIn"></param>
	/// <param name="gIn"></param>
	/// <param name="bIn"></param>
	/// <param name="rOut"></param>
	/// <param name="gOut"></param>
	/// <param name="bOut"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	static void ConvertToLab(pixel_t* rIn, pixel_t* gIn, pixel_t* bIn, pixel_t* lOut, pixel_t* aOut, pixel_t* bOut, int width, int height);

};
