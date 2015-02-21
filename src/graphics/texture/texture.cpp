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


Texture::Texture(TextureAttributes attributes)
{
	this->attributes = attributes;
}

Texture::Texture(TextureAttributes attributes, RawImage* imageData) : Texture(attributes)
{
	this->imageData.push_back(imageData);
}

Texture::Texture(TextureAttributes attributes, std::vector<RawImage *>& imageData) : Texture(attributes)
{
	this->attributes = attributes;
	this->imageData = imageData;
}

Texture::~Texture()
{
	DestroyImageData();
}

void Texture::DestroyImageData()
{
	for(unsigned int i = 0; i < imageData.size(); i++)
	{
		RawImage * raw = imageData[i];
		if(raw != NULL)
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
