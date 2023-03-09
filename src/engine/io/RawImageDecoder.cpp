#include "RawImageDecoder.h"

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
	}

	int midSizedThumbIndex = 0;
	std::vector<GeneralMetadataInternal> thumbs = std::vector<GeneralMetadataInternal>();

	//Get all thumb infos
	for (int i = 0; i < thumbCount; i++)
	{
		libraw_thumbnail_item_t thumb = processor->imgdata.thumbs_list.thumblist[i];
		GeneralMetadataInternal data = GeneralMetadataInternal();
		data.Width = thumb.twidth;
		data.Height = thumb.theight;
		data.MiscI32v1 = i;
		thumbs.push_back(data);
	}

	//Sort infos
	std::sort(thumbs.begin(), thumbs.end(), [](GeneralMetadataInternal& a, GeneralMetadataInternal& b) {
		return a.Width > b.Width;
		});

	
	int mediumPoint = std::floor(thumbCount / 2.f);

	//Find in the middle of the sorted vector
	for (size_t i = 0; i < thumbCount; i++)
	{
		if (thumbs[i].MiscI32v1 == mediumPoint) {
			midSizedThumbIndex = thumbs[i].MiscI32v1;
			break;
		}
	}
	m_ThumbnailIndex = midSizedThumbIndex;

	thumbs.clear();

	return true;
}

bool RawImageDecoder::GetPreviewImage(uint8_t* buf, GeneralMetadata& data)
{
	//Assume we have a thumbnail
	processor->unpack_thumb_ex(m_ThumbnailIndex);
	//processor->imgdata.thumbnail.




	return false;
}
