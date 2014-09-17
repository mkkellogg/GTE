/*
 * class: ImageLoader
 *
 * author: Mark Kellogg
 *
 * The OpenGL implementation of a texture.
 *
 */

#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_

//forward declarations
class RawImage;

#include <IL/il.h>

class ImageLoader
{
	public:

	static RawImage * LoadPNG(const char * fullPath);
	static RawImage * LoadImage(const char * fullPath);
	static RawImage * GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height);
};

#endif
