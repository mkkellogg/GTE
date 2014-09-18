#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "texture.h"
#include "textureattr.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>


Texture::Texture(TextureAttributes attributes)
{
	this->attributes = attributes;
}

Texture::Texture(TextureAttributes attributes, const std::string& sourcePath) : Texture(attributes)
{
	this->sourcePath = sourcePath;
}

Texture::~Texture()
{

}
