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

class ImageLoader
{
	public:

	static RawImage * LoadPNG(const char * fullPath);
};

#endif
