/*
 * class: TextureGL
 *
 * author: Mark Kellogg
 *
 * The OpenGL implementation of a texture.
 *
 */

#ifndef _GTE_TEXTURE_GL_H_
#define _GTE_TEXTURE_GL_H_

//forward declarations
class TextureGL;
class RawImage;

#include "graphics/gl_include.h"
#include "texture.h"
#include <string>

class TextureGL : public Texture
{
	friend class GraphicsGL;

	protected:

	GLuint textureID;

	TextureGL(TextureAttributes attributes, GLuint textureID);
	~TextureGL();

	public:

	GLuint GetTextureID() const;
};

#endif
