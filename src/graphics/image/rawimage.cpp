#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rawimage.h"
#include "global/global.h"
#include "debug/gtedebug.h"


RawImage::RawImage(unsigned int width, unsigned int height)
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

unsigned int RawImage::ImageSizeBytes()
{
	return width * height * 4;
}

bool RawImage::Init()
{
	imageBytes = new BYTE[ImageSizeBytes()];
	ASSERT(imageBytes != NULL, "RawImage::Init -> Unable to allocate image bytes.", false);

	return true;
}

void RawImage::SetDataTo(BYTE * data)
{
	ASSERT_RTRN(data != NULL, "RawImage::SetDataTo -> data is NULL");

	if(imageBytes != NULL)
	{
		memcpy(imageBytes, data, ImageSizeBytes());
	}
}

void RawImage::SetByte(unsigned int index, BYTE byte)
{
	if(index < ImageSizeBytes())imageBytes[index] = byte;
}

BYTE * RawImage::GetPixels()
{
	return imageBytes;
}

unsigned int RawImage::GetWidth()
{
	return width;
}

unsigned int RawImage::GetHeight()
{
	return height;
}
