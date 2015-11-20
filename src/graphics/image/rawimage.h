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

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	class RawImage
	{
		friend class ImageLoader;

	protected:

		UInt32 width;
		UInt32 height;
		Byte * imageBytes;

		UInt32 ImageSizeBytes();
		void Destroy();

	public:

		RawImage(UInt32 width, UInt32 height);
		~RawImage();

		Bool Init();
		void SetDataTo(Byte * data);
		void SetByte(UInt32 index, Byte byte);

		Byte * GetPixels();
		UInt32 GetWidth() const;
		UInt32 GetHeight() const;
	};
}

#endif
