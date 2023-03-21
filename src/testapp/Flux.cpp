#include <chrono>
#include "Flux.h"
#include "../engine/Color/Converter.h"
#include "../engine/infrastructure/LUTf3d.h"
#include "../engine/io/ImageInput.h"
#include "../engine/io/Encoders/JpegImageEncoder.h"
#include "../engine/infrastructure/PixelsHelper.h"
#include "../engine/infrastructure/BenchmarkHelper.h"
#include "../engine/infrastructure/BitmapHelper.h"
#include "../engine/io/RawImageDecoder.h"
#include <string>
using namespace std;


int main()
{
	std::string filePath = "F:\\PHOTO\\2023-02-19\\IMG_0001.CR2";
	std::string outFilePath = "F:\\IMG_0001.jpeg";
	RawImageDecoder* decoder = new RawImageDecoder(filePath);
	bool success = decoder->Init();
	GeneralMetadata data = GeneralMetadata();
	decoder->ReadGeneralMetadata(data);
	byte_t* pixels;
	success = decoder->GetPreviewImage(pixels, data);
	JpegImageEncoder* encoder = new JpegImageEncoder(pixels, data.Width, data.Height);
	success = encoder->Init();
	success = encoder->FastSave(outFilePath);
	delete[] pixels;

	//byte_t* bitmap = new byte_t[3 * 4]{ 1,2,3,0,1,2,3,0,1,2,3,0 };
	//byte_t* rgb = new byte_t[3 * 3];
	//BitmapHelper::BitmapToRgb(bitmap, rgb, 3, 3, 1);

	return 0;
}
