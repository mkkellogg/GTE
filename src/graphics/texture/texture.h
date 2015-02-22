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
class RawImage;

#include "textureattr.h"
#include "object/engineobject.h"
#include <string>

class Texture : public EngineObject
{
	friend class Graphics;

	protected:

	std::vector<RawImage *> imageData;
	TextureAttributes attributes;

	Texture(TextureAttributes attributes);
	virtual ~Texture();

	void DestroyImageData();
	void AddImageData(RawImage* imageData);

	public:

	TextureAttributes GetAttributes();
};

#endif
