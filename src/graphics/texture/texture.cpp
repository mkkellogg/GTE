#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "texture.h"
#include "textureattr.h"
#include "graphics/image/rawimage.h"
#include "graphics/image/imageloader.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <string>

namespace GTE
{
	Texture::Texture(TextureAttributes attributes)
	{
		this->attributes = attributes;
	}

	Texture::~Texture()
	{
		DestroyImageData();
	}

	void Texture::DestroyImageData()
	{
		for (unsigned int i = 0; i < imageData.size(); i++)
		{
			RawImage * raw = imageData[i];
			if (raw != NULL)
			{
				ImageLoader::DestroyRawImage(raw);
			}
		}
		imageData.clear();
	}

	TextureAttributes Texture::GetAttributes()
	{
		return attributes;
	}

	void Texture::AddImageData(RawImage* imageData)
	{
		this->imageData.push_back(imageData);
	}

	RawImage * Texture::GetImageData(unsigned int index)
	{
		NONFATAL_ASSERT_RTRN(index < imageData.size(), "Texture::GetImageData -> 'index' is out of range.", NULL, true);
		return imageData[index];
	}
}
