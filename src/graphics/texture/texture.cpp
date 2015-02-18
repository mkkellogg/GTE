#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "texture.h"
#include "textureattr.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <string>


Texture::Texture(TextureAttributes attributes)
{
	this->attributes = attributes;
}

Texture::Texture(TextureAttributes attributes, const std::string& sourcePath) : Texture(attributes)
{
	this->sourcePaths.push_back(sourcePath);
}

Texture::Texture(TextureAttributes attributes, const std::vector<std::string>& sourcePaths) : Texture(attributes)
{
	this->attributes = attributes;
	this->sourcePaths = sourcePaths;
}

Texture::~Texture()
{

}

TextureAttributes Texture::GetAttributes()
{
	return attributes;
}
