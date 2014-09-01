#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "textureattr.h"

TextureAttributes::TextureAttributes()
{
	UseAlpha = false;
	WrapMode = TextureWrap::Repeat;
	FilterMode = TextureFilter::Point;
}

TextureAttributes::~TextureAttributes()
{

}
