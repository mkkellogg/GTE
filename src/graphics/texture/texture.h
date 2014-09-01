/*
 * class: Texture
 *
 * author: Mark Kellogg
 *
 * The platform independent base class for textures.
 *
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

//forward declarations
class Graphics;
class TextureAttributes;

#include "textureattr.h"

class Texture
{
	friend class Graphics;

	protected:

	char * sourcePath;
	TextureAttributes attributes;

	public:

	Texture(TextureAttributes attributes);
	Texture(TextureAttributes attributes, const char * sourcePath);
	virtual ~Texture();
};

#endif
