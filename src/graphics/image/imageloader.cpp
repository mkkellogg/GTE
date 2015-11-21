#include "imageloader.h"
#include "rawimage.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"
#include <IL/il.h>

namespace GTE
{
	Bool ImageLoader::ilInitialized = false;

	Bool ImageLoader::Initialize()
	{
		if (!ImageLoader::ilInitialized)
		{
			ASSERT(ilGetInteger(IL_VERSION_NUM) >= IL_VERSION, "AssetImporter::ProcessMaterials -> wrong DevIL version");

			ilInit(); /// Initialization of DevIL
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
			ilEnable(IL_ORIGIN_SET);
			ImageLoader::ilInitialized = true;
		}

		return true;
	}

	RawImage * ImageLoader::LoadImageU(const std::string& fullPath)
	{
		return LoadImageU(fullPath, false);
	}

	RawImage * ImageLoader::LoadImageU(const std::string& fullPath, Bool reverseOrigin)
	{
		Bool initializeSuccess = Initialize();
		NONFATAL_ASSERT_RTRN(initializeSuccess, "ImageLoader::LoadImage -> Error occurred while initializing image loader.", nullptr, false);

		if(reverseOrigin)
		{
			ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		}

		std::string extension = GetFileExtension(fullPath);

		ILuint imageIds[1];
		ilGenImages(1, imageIds); //Generation of numTextures image names
		ilBindImage(imageIds[0]); // Binding of DevIL image name
		RawImage * rawImage = nullptr;

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
				return nullptr;
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
			return nullptr;
		}

		// Because we have already copied image data into texture data we can release memory used by image.
		ilDeleteImages(1, imageIds);

		if(reverseOrigin)
		{
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		}

		return rawImage;
	}

	RawImage * ImageLoader::GetRawImageFromILData(const ILubyte * data, UInt32 width, UInt32 height)
	{
		NONFATAL_ASSERT_RTRN(data != nullptr, "ImportUtil::GetRawImageFromILData -> 'data' is null.", nullptr, true);

		RawImage * rawImage = new(std::nothrow) RawImage(width, height);
		ASSERT(rawImage != nullptr, "ImportUtil::GetRawImageFromILData -> Could not allocate RawImage.");

		Bool initSuccess = rawImage->Init();
		if (!initSuccess)
		{
			Debug::PrintError("ImportUtil::GetRawImageFromILData -> Could not init RawImage.");
			delete rawImage;
			return nullptr;
		}

		for (UInt32 i = 0; i < width * height * 4; i++)
		{
			rawImage->SetByte(i, (Byte)data[i]);
		}

		return rawImage;
	}

	void ImageLoader::DestroyRawImage(RawImage * image)
	{
		NONFATAL_ASSERT(image != nullptr, "ImageLoader::DestroyRawImage -> 'image' is null.", true);
		delete image;
	}

	std::string ImageLoader::GetFileExtension(const std::string& filePath)
	{
		Int32 dotIndex = (Int32)filePath.find_last_of(".");
		if (dotIndex < 0)dotIndex = 0;
		std::string extension = filePath.substr(dotIndex, filePath.size() - dotIndex);
		return extension;
	}
}
