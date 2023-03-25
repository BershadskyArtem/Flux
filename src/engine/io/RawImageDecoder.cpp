#include "RawImageDecoder.h"

std::mutex RawImageDecoder::s_OpenRawMutex = std::mutex();
std::mutex RawImageDecoder::s_UnpackRawMutex = std::mutex();
std::mutex RawImageDecoder::s_UnpackThumbMutex = std::mutex();


bool RawImageDecoder::GetHalfSizedRaw(uint8_t* buf)
{
	return false;
}

bool RawImageDecoder::GetPreviewFromJpeg(unsigned char* inMemoryJpeg, long inMemoryJpegSize, uint8_t* buf)
{
	JpegImageDecoder* decoder = new JpegImageDecoder(inMemoryJpeg, inMemoryJpegSize);
	bool success = decoder->Init();
	if (!success) {
		m_HasErrors = true;
		return !HasErrors();
	}

	success = decoder->GetPreviewImage(buf);
	if (!success) {
		m_HasErrors = true;
		return !HasErrors();
	}
	delete decoder;
	return true;
}

bool RawImageDecoder::GetPreviewFromBitmap(byte_t* bitmap, int width, int height, int channelsCount, uint8_t* buf)
{
	buf = new uint8_t[width * height * channelsCount];
	BitmapHelper::BitmapToRgb(bitmap, buf, width, height, 3);
	return true;
}

bool RawImageDecoder::IsSupportedThumbnailformat(LibRaw_thumbnail_formats format)
{
	switch (format)
	{
	case LIBRAW_THUMBNAIL_UNKNOWN:
		return false;
		break;
	case LIBRAW_THUMBNAIL_JPEG:
		return true;
		break;
	case LIBRAW_THUMBNAIL_BITMAP:
		return true;
		break;
	case LIBRAW_THUMBNAIL_BITMAP16:
		return false;
		break;
	case LIBRAW_THUMBNAIL_LAYER:
		return false;
		break;
	case LIBRAW_THUMBNAIL_ROLLEI:
		return false;
		break;
	case LIBRAW_THUMBNAIL_H265:
		return false;
		break;
	default:
		break;
	}
}

bool RawImageDecoder::IsSupportedThumbnailformat(LibRaw_internal_thumbnail_formats format)
{
	switch (format)
	{
	case LIBRAW_INTERNAL_THUMBNAIL_UNKNOWN:
		return false;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_KODAK_THUMB:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_KODAK_YCBCR:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_KODAK_RGB:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_JPEG:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_LAYER:
		return false;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_ROLLEI:
		return false;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_PPM:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_PPM16:
		return true;
		break;
	case LIBRAW_INTERNAL_THUMBNAIL_X3F:
		return true;
		break;
	default:
		return false;
		break;
	}
}

RawImageDecoder::RawImageDecoder(std::string fileName) : AbstractImageDecoder(fileName)
{
}

bool RawImageDecoder::ReadGeneralMetadata(GeneralMetadata& data)
{
	data.Width = m_Width;
	data.Height = m_Height;
	return !HasErrors();
}

bool RawImageDecoder::ReadPreviewGeneralMetadata(GeneralMetadata& data)
{
	data.Width = m_ScaledWidth;
	data.Height = m_ScaledHeight;
	return !HasErrors();
}

bool RawImageDecoder::ReadExifMetadata(ExifMetadata& data)
{
	return false;
}

bool RawImageDecoder::ReadIptcMetadata(IptcMetadata& data)
{
	return false;
}

bool RawImageDecoder::ReadMakerMetadata(MakerMetadata& data)
{
	return false;
}

bool RawImageDecoder::Init()
{
	s_OpenRawMutex.lock();
	std::cout << "Lock" << std::endl;
	processor = new LibRaw();
	int errorCode = processor->open_file(m_fileName.c_str());
	s_OpenRawMutex.unlock();
	

	if (errorCode != 0) {
		m_HasErrors = true;
		
		return !HasErrors();
	}
	m_Width = processor->imgdata.sizes.width;
	m_Height = processor->imgdata.sizes.height;

	processor->imgdata.params.output_bps = 16;

	//if (false) {
	//	processor->imgdata.params.output_color = 1;
	//	processor->imgdata.params.gamm[0] = 1.0 / 2.4;
	//	processor->imgdata.params.gamm[1] = 12.92;
	//}
	//else {
		processor->imgdata.params.output_color = 0;
		processor->imgdata.params.gamm[0] = 1.0f;
		processor->imgdata.params.gamm[1] = 1.0f;
	//}

	
	processor->imgdata.params.use_auto_wb = false;
	processor->imgdata.params.use_camera_wb = false;
	//Always use camera color space
	processor->imgdata.params.use_camera_matrix = 3;
	
	//Use Modified AHD interpolation (by Anton Petrusevich)
	processor->imgdata.params.user_qual = 12;
	processor->imgdata.params.no_auto_bright = true;

	

	//Find Mid-sized preview
	int thumbCount = processor->imgdata.thumbs_list.thumbcount;
	//std::cout << thumbCount << std::endl;

	if (thumbCount == 0) {
		m_HasThumbnail = false;
	
		return !HasErrors();
	}

	int midSizedThumbIndex = 0;
	std::vector<GeneralMetadataInternal> thumbs = std::vector<GeneralMetadataInternal>();

	//Get all thumb infos
	for (int i = 0; i < thumbCount; i++)
	{
		libraw_thumbnail_item_t thumb = processor->imgdata.thumbs_list.thumblist[i];
		GeneralMetadataInternal data = GeneralMetadataInternal();

		if (!IsSupportedThumbnailformat(thumb.tformat))
			continue;
		data.Width = thumb.twidth;
		data.Height = thumb.theight;
		data.MiscI32v1 = i;
		thumbs.push_back(data);
	}

	//Sort infos
	std::sort(thumbs.begin(), thumbs.end(), [](GeneralMetadataInternal& a, GeneralMetadataInternal& b) {
		return a.Width > b.Width;
		});

	int targetPreview = 0;

	//int mediumPoint = (int)(thumbs.size() / 2.f);
	//
	//if (thumbs.size() <= 2) {
	//	mediumPoint = thumbs.size() - 1;
	//}

	//Find in the middle of the sorted vector
	//for (size_t i = 0; i < thumbCount; i++)
	//{
	//	if (thumbs[i].MiscI32v1 == mediumPoint) {
	//		midSizedThumbIndex = thumbs[i].MiscI32v1;
	//		break;
	//	}
	//}
	//m_ThumbnailIndex = midSizedThumbIndex;
	//m_ThumbnailIndex = thumbs[mediumPoint].MiscI32v1;
	//m_ThumbnailIndex = thumbs[thumbs.size() - 1].MiscI32v1;
	m_ThumbnailIndex = thumbs[targetPreview].MiscI32v1;
	m_HasThumbnail = true;
	m_ScaledHeight = thumbs[targetPreview].Height / 2;
	m_ScaledWidth = thumbs[targetPreview].Width / 2;
	thumbs.clear();

	return !HasErrors();
}

bool RawImageDecoder::GetPreviewImage(uint8_t* buf)
{
	//If we has errors we exit with false(fail) flag
	if (HasErrors()) {
		
		return !HasErrors();
	}

	if (!m_HasThumbnail) {
		std::cout << "Failed" << std::endl;
		return false;
	}

	//s_UnpackThumbMutex.lock();

	int err = processor->unpack_thumb_ex(m_ThumbnailIndex);


	//s_UnpackThumbMutex.unlock();

	if (err != LIBRAW_SUCCESS)
	{
		
		m_HasErrors = true;
		m_HasThumbnail = false;
		return !HasErrors();
	}

	//Get thumb info
	LibRaw_thumbnail_formats thumbFormat = processor->imgdata.thumbnail.tformat;
	long thumbSize = processor->imgdata.thumbnail.tlength;
	unsigned char* thumbPtr = (unsigned char*)processor->imgdata.thumbnail.thumb;
	int thumbWidth = processor->imgdata.thumbnail.twidth;
	int thumbHeight = processor->imgdata.thumbnail.theight;
	int channelCount = processor->imgdata.thumbnail.tcolors;

	switch (thumbFormat)
	{
	case LIBRAW_THUMBNAIL_UNKNOWN:
		m_HasErrors = true;
		return !HasErrors();
		break;
	case LIBRAW_THUMBNAIL_JPEG:
		return this->GetPreviewFromJpeg(thumbPtr, thumbSize, buf);
		break;
	case LIBRAW_THUMBNAIL_BITMAP:
		return this->GetPreviewFromBitmap(thumbPtr, thumbWidth, thumbHeight, channelCount, buf);
		break;
	case LIBRAW_THUMBNAIL_BITMAP16:
		return this->GetPreviewFromBitmap(thumbPtr, thumbWidth, thumbHeight, channelCount, buf);
		break;
	case LIBRAW_THUMBNAIL_LAYER:
		m_HasErrors = true;
		return !HasErrors();
		break;
	case LIBRAW_THUMBNAIL_ROLLEI:
		m_HasErrors = true;
		return !HasErrors();
		break;
	case LIBRAW_THUMBNAIL_H265:
		m_HasErrors = true;
		return !HasErrors();
		break;
	default:
		return false;
		break;
	}
}

bool RawImageDecoder::GetFullImage(float* buff)
{
	int success = 0;

	//s_UnpackRawMutex.lock();
	success = processor->unpack();
	//s_UnpackRawMutex.unlock();

	if (success != 0)
	{
		m_HasErrors = true;
		return !HasErrors();
	}

	m_Unpacked = true;
	
	if (buff == nullptr) {
		return false;
	}

	if (m_Image == nullptr) {
		processor->dcraw_process();
		m_Image = processor->dcraw_make_mem_image(&success);
		if (success != 0)
		{
			m_HasErrors = true;
			return !HasErrors();
		}
	}
	
	if (m_Image->colors != 3) {
		m_HasErrors = true;
		return !HasErrors();
	}

	//From https://github.com/OpenImageIO/oiio/blob/8e9b334a255148d8f435b617e98cbc90a0cf99a7/src/raw.imageio/rawinput.cpp
	// line 1577
#pragma omp parallel for
	for (int y = 0; y < m_Height; y++)
	{
		uint16_t* rowStart = &(((unsigned short*)m_Image->data)[m_Width * 3 * y]);
		int rowIndexTarget = y * m_Width * 3;
		for (int x = 0; x < m_Width; x++)
		{
			buff[(rowIndexTarget + x * 3) + 0] = rowStart[x * 3 + 0] / 65536.0f;
			buff[(rowIndexTarget + x * 3) + 1] = rowStart[x * 3 + 1] / 65536.0f;
			buff[(rowIndexTarget + x * 3) + 2] = rowStart[x * 3 + 2] / 65536.0f;
		}
	}
	return true;
}

int RawImageDecoder::ScaledWidth()
{
	return m_ScaledWidth;
}

int RawImageDecoder::ScaledHeight()
{
	return m_ScaledHeight;
}

RawImageDecoder::~RawImageDecoder()
{
	processor->recycle();
	delete processor;

	if (m_Image != nullptr) {
		delete m_Image;
	}
}
