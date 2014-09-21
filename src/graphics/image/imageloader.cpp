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

bool ImageLoader::ilInitialized = false;

bool ImageLoader::Initialize()
{
	if(!ImageLoader::ilInitialized)
	{
		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
		{
			/// wrong DevIL version ///
			Debug::PrintError("AssetImporter::ProcessMaterials -> wrong DevIL version");
			return false;
		}

		ilInit(); /// Initialization of DevIL
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ImageLoader::ilInitialized = true;
	}

	return true;
}

RawImage * ImageLoader::LoadPNG(const std::string& fullPath)
{
	if(!Initialize())
	{
		Debug::PrintError(" ImageLoader::LoadPNG -> Error occurred while initializing image loader.");
		return NULL;
	}

	std::vector<unsigned char> image; // raw pixels
	unsigned width, height;

	//the raw pixels will be in the vector "image", 4 bytes per pixel, ordered RGBARGBA
	unsigned error = lodepng::decode(image, width, height, fullPath);

	if(error)
	{
		std::string loadErr = std::string("Error loading PNG: ") + lodepng_error_text(error);
		Debug::PrintError(loadErr);
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

RawImage * ImageLoader::LoadImage(const std::string& fullPath)
{
	if(!Initialize())
	{
		Debug::PrintError(" ImageLoader::LoadImage -> Error occurred while initializing image loader.");
		return NULL;
	}

	std::string extension = GetFileExtension(fullPath);

	ILuint imageIds[1];
	ilGenImages(1, imageIds); //Generation of numTextures image names
	ilBindImage(imageIds[0]); // Binding of DevIL image name
	RawImage * rawImage = NULL;

	ILboolean success = ilLoadImage(fullPath.c_str());

	if (success) // If no error occurred:
	{
		// Convert every color component into unsigned byte.If your image contains
		// alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			// Error occurred
			Debug::PrintError("ImageLoader::LoadImage -> Couldn't convert image");
			ilDeleteImages(1, imageIds);
			return NULL;
		}

		rawImage = GetRawImageFromILData(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT));
	}
	else
	{
		ILenum i = ilGetError();
		std::string msg = "ImageLoader::LoadImage -> Couldn't load image: ";
		msg += fullPath.c_str();
		if(i == IL_INVALID_EXTENSION)
		{
			msg = std::string("ImageLoader::LoadImage -> Couldn't load image (invalid extension). ");
			msg += std::string("Is DevIL configured to load extension: ") + extension + std::string(" ?");
		}

		Debug::PrintError(msg);
		ilDeleteImages(1, imageIds);
		return NULL;
	}

	// Because we have already copied image data into texture data we can release memory used by image.
	ilDeleteImages(1, imageIds);
	return rawImage;
}

RawImage * ImageLoader::GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height)
{
	NULL_CHECK(data,"ImportUtil::GetRawImageFromILData -> data is NULL.",NULL);

	RawImage * rawImage = new RawImage(width, height);
	NULL_CHECK(rawImage,"ImportUtil::GetRawImageFromILData -> Could not allocate RawImage.",NULL);

	bool initSuccess = rawImage->Init();
	if(!initSuccess)
	{
		Debug::PrintError("ImportUtil::GetRawImageFromILData -> Could not init RawImage.");
		delete rawImage;
		return NULL;
	}

	for(unsigned int i=0; i < width * height * 4; i++)
	{
		rawImage->SetByte(i,(BYTE)data[i]);
	}

	return rawImage;
}

void ImageLoader::DestroyRawImage(RawImage * image)
{
	NULL_CHECK_RTRN(image,"ImageLoader::DestroyRawImage -> image is NULL.");
	delete image;
}

std::string ImageLoader::GetFileExtension(const std::string& filePath)
{
	int dotIndex = filePath.find_last_of(".");
	if(dotIndex <0)dotIndex=0;
	std::string extension = filePath.substr(dotIndex,filePath.size()-dotIndex);
	return extension;
}
