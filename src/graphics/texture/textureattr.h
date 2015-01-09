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
	BiLinear,
	TriLinear,
	Anisotropic
};

class TextureAttributes
{
	public:

	unsigned int MipMapLevel;
	bool UseAlpha;
	TextureFilter FilterMode;
	TextureWrap WrapMode;

	TextureAttributes();
	~TextureAttributes();
};

#endif
