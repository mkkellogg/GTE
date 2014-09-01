#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "texture.h"
#include "textureattr.h"
#include "global/global.h"
#include "ui/debug.h"


Texture::Texture(TextureAttributes attributes)
{
	sourcePath = NULL;
	this->attributes = attributes;
}

Texture::Texture(TextureAttributes attributes, const char * sourcePath) : Texture(attributes)
{
	if(sourcePath != NULL)
	{
		this->sourcePath = new char[strlen(sourcePath)+1];
		strcpy(this->sourcePath, sourcePath);
	}
}

Texture::~Texture()
{
	SAFE_DELETE(sourcePath);
}
