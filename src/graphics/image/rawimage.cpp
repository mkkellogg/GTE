#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rawimage.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	RawImage::RawImage(UInt32 width, UInt32 height)
	{
		this->width = width;
		this->height = height;
		imageBytes = NULL;
	}

	RawImage::~RawImage()
	{
		Destroy();
	}

	void RawImage::Destroy()
	{
		SAFE_DELETE(imageBytes);
	}

	UInt32 RawImage::ImageSizeBytes()
	{
		return width * height * 4;
	}

	Bool RawImage::Init()
	{
		imageBytes = new Byte[ImageSizeBytes()];
		ASSERT(imageBytes != NULL, "RawImage::Init -> Unable to allocate image bytes.");

		return true;
	}

	void RawImage::SetDataTo(Byte * data)
	{
		NONFATAL_ASSERT(data != NULL, "RawImage::SetDataTo -> 'data' is null.", true);

		if (imageBytes != NULL)
		{
			memcpy(imageBytes, data, ImageSizeBytes());
		}
	}

	void RawImage::SetByte(UInt32 index, Byte byte)
	{
		if (index < ImageSizeBytes())imageBytes[index] = byte;
	}

	Byte * RawImage::GetPixels()
	{
		return imageBytes;
	}

	UInt32 RawImage::GetWidth()
	{
		return width;
	}

	UInt32 RawImage::GetHeight()
	{
		return height;
	}
}
