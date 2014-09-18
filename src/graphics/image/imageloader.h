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
#include <string>

class ImageLoader
{
	static bool ilInitialized;
	static bool Initialize();

	public:

	static RawImage * LoadPNG(const std::string& fullPath);
	static RawImage * LoadImage(const std::string& fullPath);
	static RawImage * GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height);
};

#endif
