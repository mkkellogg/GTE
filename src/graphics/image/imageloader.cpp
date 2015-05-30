#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "imageloader.h"
#include "rawimage.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <IL/il.h>

namespace GTE
{
	bool ImageLoader::ilInitialized = false;

	bool ImageLoader::Initialize()
	{
		if (!ImageLoader::ilInitialized)
		{
			ASSERT(ilGetInteger(IL_VERSION_NUM) >= IL_VERSION, "AssetImporter::ProcessMaterials -> wrong DevIL version");

			ilInit(); /// Initialization of DevIL
			ilEnable(IL_ORIGIN_SET);
			ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
			ImageLoader::ilInitialized = true;
		}

		return true;
	}

	RawImage * ImageLoader::LoadImageU(const std::string& fullPath)
	{
		bool initializeSuccess = Initialize();
		NONFATAL_ASSERT_RTRN(initializeSuccess, "ImageLoader::LoadImage -> Error occurred while initializing image loader.", NULL, false);

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
				Engine::Instance()->GetErrorManager()->SetAndReportError(ImageLoaderError::GeneralLoadError, "ImageLoader::LoadImage -> Couldn't convert image");
				ilDeleteImages(1, imageIds);
				return NULL;
			}

			rawImage = GetRawImageFromILData(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		}
		else
		{
			ILenum i = ilGetError();
			std::string msg = "ImageLoader::LoadImage -> Couldn't load image: ";
			msg += fullPath.c_str();
			if (i == IL_INVALID_EXTENSION)
			{
				msg = std::string("ImageLoader::LoadImage -> Couldn't load image (invalid extension). ");
				msg += std::string("Is DevIL configured to load extension: ") + extension + std::string(" ?");
			}

			Engine::Instance()->GetErrorManager()->SetAndReportError(ImageLoaderError::GeneralLoadError, msg);
			ilDeleteImages(1, imageIds);
			return NULL;
		}

		// Because we have already copied image data into texture data we can release memory used by image.
		ilDeleteImages(1, imageIds);
		return rawImage;
	}

	RawImage * ImageLoader::GetRawImageFromILData(ILubyte * data, unsigned int width, unsigned int height)
	{
		NONFATAL_ASSERT_RTRN(data != NULL, "ImportUtil::GetRawImageFromILData -> 'data' is null.", NULL, true);

		RawImage * rawImage = new RawImage(width, height);
		ASSERT(rawImage != NULL, "ImportUtil::GetRawImageFromILData -> Could not allocate RawImage.");

		bool initSuccess = rawImage->Init();
		if (!initSuccess)
		{
			Debug::PrintError("ImportUtil::GetRawImageFromILData -> Could not init RawImage.");
			delete rawImage;
			return NULL;
		}

		for (unsigned int i = 0; i < width * height * 4; i++)
		{
			rawImage->SetByte(i, (BYTE)data[i]);
		}

		return rawImage;
	}

	void ImageLoader::DestroyRawImage(RawImage * image)
	{
		NONFATAL_ASSERT(image != NULL, "ImageLoader::DestroyRawImage -> 'image' is null.", true);
		delete image;
	}

	std::string ImageLoader::GetFileExtension(const std::string& filePath)
	{
		int dotIndex = filePath.find_last_of(".");
		if (dotIndex < 0)dotIndex = 0;
		std::string extension = filePath.substr(dotIndex, filePath.size() - dotIndex);
		return extension;
	}
}
