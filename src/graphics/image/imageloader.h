/*
 * class: ImageLoader
 *
 * author: Mark Kellogg
 *
 * Utility functions for loading 2D images. Mostly this acts as a
 * middleman for the DevIL image loading library.
 *
 */

#ifndef _GTE_IMAGE_LOADER_H_
#define _GTE_IMAGE_LOADER_H_

//forward declarations
class RawImage;

#include <IL/il.h>
#include <string>

enum ImageLoaderError
{
	GeneralLoadError = 1
};

class ImageLoader
{
	static bool ilInitialized;
	static bool Initialize();

	public:

	static RawImage * LoadPNG(const std::string& fullPath);
	static RawImage * LoadImage(const std::string& fullPath);
	static RawImage * GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height);
	static void DestroyRawImage(RawImage * image);
	static std::string GetFileExtension(const std::string& filePath);
};

#endif
