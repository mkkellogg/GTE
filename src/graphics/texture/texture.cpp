#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "texture.h"
#include "textureattr.h"
#include "graphics/image/rawimage.h"
#include "graphics/image/imageloader.h"
#include "global/global.h"
#include "global/assert.h"
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
		for (UInt32 i = 0; i < imageData.size(); i++)
		{
			RawImage * raw = imageData[i];
			if (raw != nullptr)
			{
				ImageLoader::DestroyRawImage(raw);
			}
		}
		imageData.clear();
	}

	TextureAttributes Texture::GetAttributes() const
	{
		return attributes;
	}

	void Texture::AddImageData(RawImage* imageData)
	{
		this->imageData.push_back(imageData);
	}

	RawImage * Texture::GetImageData(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < imageData.size(), "Texture::GetImageData -> 'index' is out of range.", nullptr, true);
		return imageData[index];
	}
}
