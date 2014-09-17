#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "imageloader.h"
#include "rawimage.h"
#include "lodepng/lodepng.h"
#include "lodepng/lodepng_util.h"
#include "global/global.h"
#include "ui/debug.h"
#include <IL/il.h>

RawImage * ImageLoader::LoadPNG(const char * fullPath)
{
	std::vector<unsigned char> image; // raw pixels
	unsigned width, height;

	//the raw pixels will be in the vector "image", 4 bytes per pixel, ordered RGBARGBA
	unsigned error = lodepng::decode(image, width, height, fullPath);

	if(error)
	{
		char msg[64];
		const char * loadErr = lodepng_error_text(error);
		sprintf(msg, "Error loading PNG: %s", loadErr);
		Debug::PrintError(msg);
		return NULL;
	}

	RawImage * raw = new RawImage(width,height);
	bool initSuccess = raw->Init();

	if(!initSuccess)
	{
		Debug::PrintError("ImageLoader::LoadPNG -> could not init raw image");
		return NULL;
	}

	for(unsigned int i=0; i < width * height * 4; i++)
	{
		raw->SetByte(i,(BYTE)image[i]);
	}

	return raw;
}

RawImage * ImageLoader::LoadImage(const char * fullPath)
{
	ILuint imageIds[1];
	ilGenImages(1, imageIds); //Generation of numTextures image names

	//save IL image ID
	std::string filename = fullPath; // get filename

	ilBindImage(imageIds[0]); // Binding of DevIL image name
	std::string fileloc = fullPath;	// Loading of image
	RawImage * rawImage = NULL;
	ILboolean success = ilLoadImage(fileloc.c_str());
	if (success) // If no error occured:
	{
		// Convert every colour component into unsigned byte.If your image contains
		// alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			// Error occured
			Debug::PrintError("AssetImporter::ProcessMaterials -> Couldn't convert image");
			ilDeleteImages(1, imageIds);
			return NULL;
		}

		/*
		ilGetInteger(IL_IMAGE_BPP),
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		ilGetInteger(IL_IMAGE_FORMAT),
		ilGetData()
		*/

		rawImage = GetRawImageFromILData(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT));
	}

	// Because we have already copied image data into texture data we can release memory used by image.
	ilDeleteImages(1, imageIds);

	return rawImage;
}

RawImage * ImageLoader::GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height)
{
	RawImage * rawImage = new RawImage(width, height);
	NULL_CHECK(rawImage,"ImportUtil::GetRawImageFromILData -> Could not allocate RawImage.",NULL);

	bool initSuccess = rawImage->Init();
	if(!initSuccess)
	{
		Debug::PrintError("ImportUtil::GetRawImageFromILData -> Could not init RawImage.");
		return NULL;
	}

	for(unsigned int i=0; i < width * height * 4; i++)
	{
		rawImage->SetByte(i,(BYTE)data[i]);
	}

	return rawImage;
}
