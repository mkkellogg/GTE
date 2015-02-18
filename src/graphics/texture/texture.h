/*
 * class: Texture
 *
 * author: Mark Kellogg
 *
 * The platform independent base class for textures.
 *
 */

#ifndef _GTE_TEXTURE_H_
#define _GTE_TEXTURE_H_

//forward declarations
class Graphics;
class TextureAttributes;

#include "textureattr.h"
#include "object/engineobject.h"
#include <string>

class Texture : public EngineObject
{
	friend class Graphics;

	protected:

	std::vector<std::string> sourcePaths;
	TextureAttributes attributes;

	Texture(TextureAttributes attributes);
	Texture(TextureAttributes attributes, const std::string& sourcePath);
	Texture(TextureAttributes attributes, const std::vector<std::string>& sourcePaths);
	virtual ~Texture();

	public:

	TextureAttributes GetAttributes();
};

#endif
