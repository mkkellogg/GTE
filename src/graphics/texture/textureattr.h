/*
 *
 * author: Mark Kellogg
 *
 * TextureAttributes is a descriptor that describes how a
 * given texture should be loaded and rendered.
 *
 */

#ifndef _GTE_TEXTUREATTR_H_
#define _GTE_TEXTUREATTR_H_

//forward declarations

#include "global/global.h"

enum class TextureWrap
{
	Repeat,
	Clamp,
	Mirror
};

enum class TextureFilter
{
	Point,
	Linear,
	BiLinear,
	TriLinear,
	Anisotropic
};

enum class TextureFormat
{
	RGBA8,
	RGBA128
};


class TextureAttributes
{
	public:

	unsigned int MipMapLevel;
	bool IsDepthTexture;
	bool UseAlpha;
	bool IsCube;
	TextureFilter FilterMode;
	TextureWrap WrapMode;
	TextureFormat Format;

	TextureAttributes();
	~TextureAttributes();
};

#endif
