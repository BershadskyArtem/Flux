#include "RawImageDecoder.h"

bool RawImageDecoder::GetHalfSizedRaw(uint8_t*& buf, GeneralMetadata& data)
{
	return false;
}

bool RawImageDecoder::GetPreviewFromJpeg(unsigned char* inMemoryJpeg, long inMemoryJpegSize, uint8_t* &buf, GeneralMetadata& data)
{
	JpegImageDecoder* decoder = new JpegImageDecoder(inMemoryJpeg, inMemoryJpegSize);
	bool success = decoder->Init();
	if (!success) {
		m_HasErrors = true;
		return !HasErrors();
	}

	success = decoder->GetPreviewImage(buf, data);
	if (!success) {
		m_HasErrors = true;
		return !HasErrors();
	}
	delete decoder;
	return true;
}

bool RawImageDecoder::GetPreviewFromBitmap(byte_t* bitmap, int width, int height, int channelsCount, uint8_t* &buf, GeneralMetadata& data)
{
	buf = new uint8_t[width * height * channelsCount];
	BitmapHelper::BitmapToRgb(bitmap, buf, width, height,3);
	data.Width = width;
	data.Height = height;
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
	if (HasErrors()) {
		return false;
	}
	data.Width = m_Width;
	data.Height = m_Height;
	return true;
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
	processor = new LibRaw();
	int errorCode = processor->open_file(m_fileName.c_str());
	if (errorCode != 0) {
		m_HasErrors = true;
		return !HasErrors();
	}
	m_Width = processor->imgdata.sizes.width;
	m_Height = processor->imgdata.sizes.height;


	//Find Mid-sized preview
	int thumbCount = processor->imgdata.thumbs_list.thumbcount;


	if (thumbCount == 0) {
		m_HasThumbnail = false;
		return true;
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
	m_ThumbnailIndex = thumbs[0].MiscI32v1;

	thumbs.clear();

	return true;
}

bool RawImageDecoder::GetPreviewImage(uint8_t* &buf, GeneralMetadata& data)
{
	//Assume we have a thumbnail
	int err = processor->unpack_thumb_ex(m_ThumbnailIndex);

	if (err != LIBRAW_SUCCESS)
	{
		m_HasErrors = true;
		m_HasThumbnail = false;
		return !HasErrors();
	}

	//processor->imgdata.thumbnail.

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
		return this->GetPreviewFromJpeg(thumbPtr, thumbSize, buf, data);
		break;
	case LIBRAW_THUMBNAIL_BITMAP:
		this->GetPreviewFromBitmap(thumbPtr, thumbWidth, thumbHeight, channelCount, buf, data);
		break;
	case LIBRAW_THUMBNAIL_BITMAP16:
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
		break;
	}

	return false;
}

bool RawImageDecoder::GetFullImage(float* buff)
{
	return false;
}

int RawImageDecoder::ScaledWidth()
{
	return 0;
}

int RawImageDecoder::ScaledHeight()
{
	return 0;
}

RawImageDecoder::~RawImageDecoder()
{
	processor->recycle();
	delete processor;
}
