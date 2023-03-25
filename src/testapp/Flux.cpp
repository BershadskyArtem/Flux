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
	
	//std::string filePath = "F:\\PHOTO\\2023-02-19\\IMG_0001.CR2";
	//std::string outFilePath = "F:\\IMG_0001.jpeg";
	

	std::string inputFolder = "C:\\Users\\Artyom\\Desktop\\RawImageSamples";


	std::string files[] = { "DSC_0420.NEF" , "sample1.cr2", "Y-JG-CANON-EOS-R7-0011.CR3", "Y-JG-CANON-EOS-R7-0095.CR3", "DSC02077.ARW", "IMGP0763.DNG", "DSCF3655.RAF", "Canon - PowerShot SX260 HS - 12bit (4 3).CRW"};


//#pragma omp parallel for
	for (int i = 0; i < 8; i++)
	{
		std::string fileN = files[i];
		std::cout << "Preview extraction started for file: " << fileN << std::endl;
		std::string filePath = inputFolder + "\\" + fileN;
		std::string outFilePath = "F:\\IMG_000" + std::to_string(i + 2) + ".jpeg";


		auto start = BenchmarkHelper::StartWatch();
		ImageInput* input = new ImageInput(filePath);
		bool success = input->Init();

		if (!success)
		{
			delete input;

			std::cout << "Failed to init decoder." << std::endl;
			std::cout << std::endl;
			continue;
		}

		GeneralMetadata* data = input->ReadPreviewGeneralMetadata();

		if (data == nullptr) {
			delete data;
			delete input;
			std::cout << "Failed to load preview metadata." << std::endl;
			std::cout << std::endl;
			continue;
		}

		auto image = input->GetPreview();

		if (image == nullptr) {
			delete data;
			delete input;
			std::cout << "Failed to load preview pixels." << std::endl;
			std::cout << std::endl;
			continue;
		}

		BenchmarkHelper::ShowDurationFinal(start, "Preview extracted");

		JpegImageEncoder* encoder = new JpegImageEncoder((byte_t*)(image->Pixels), image->Width, image->Height);
		success = encoder->Init();

		if (!success) {
			ImageInput::FreeFluxImage(image);
			delete input;
			delete data;
			delete encoder;
			std::cout << "Failed to init jpeg encoder." << std::endl;
			std::cout << std::endl;
			continue;
		}

		success = encoder->FastSave(outFilePath);

		if (!success) {
			ImageInput::FreeFluxImage(image);
			delete input;
			delete data;
			delete encoder;
			std::cout << "Failed to save jpeg file." << std::endl;
			std::cout << std::endl;
			continue;
		}


		ImageInput::FreeFluxImage(image);
		delete input;
		delete encoder;
		delete data;

		std::cout << std::endl;
	}




	

	return 0;
}
