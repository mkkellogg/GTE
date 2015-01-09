/*
 * class: RawImage
 *
 * author: Mark Kellogg
 *
 * A container for raw 2D image data.
 *
 */

#ifndef _GTE_RAWIMAGE_H_
#define _GTE_RAWIMAGE_H_

//forward declarations

#include "global/global.h"

class RawImage
{
	protected:

	unsigned int width;
	unsigned int height;
	BYTE * imageBytes;

	unsigned int ImageSizeBytes();
	void Destroy();

	public:

	RawImage(unsigned int width, unsigned int height);
	~RawImage();

	bool Init();
	void SetDataTo(BYTE * data);
	void SetByte(unsigned int index, BYTE byte);
	BYTE * GetPixels();

	unsigned int GetWidth();
	unsigned int GetHeight();
};

#endif
