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
