#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "textureattr.h"

namespace GTE
{
	TextureAttributes::TextureAttributes()
	{
		UseAlpha = false;
		WrapMode = TextureWrap::Repeat;
		FilterMode = TextureFilter::Point;
		MipMapLevel = 1;
		IsCube = false;
		IsDepthTexture = false;
		Format = TextureFormat::RGBA8;
	}

	TextureAttributes::~TextureAttributes()
	{

	}
}
