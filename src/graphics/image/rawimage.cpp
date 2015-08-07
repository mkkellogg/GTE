#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rawimage.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "object/enginetypes.h"
#include "engine.h"

namespace GTE
{
	RawImage::RawImage(UInt32 width, UInt32 height)
	{
		this->width = width;
		this->height = height;
		imageBytes = nullptr;
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
		imageBytes = new(std::nothrow) Byte[ImageSizeBytes()];
		ASSERT(imageBytes != nullptr, "RawImage::Init -> Unable to allocate image bytes.");

		return true;
	}

	void RawImage::SetDataTo(Byte * data)
	{
		NONFATAL_ASSERT(data != nullptr, "RawImage::SetDataTo -> 'data' is null.", true);

		if (imageBytes != nullptr)
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

	UInt32 RawImage::GetWidth() const
	{
		return width;
	}

	UInt32 RawImage::GetHeight() const
	{
		return height;
	}
}
