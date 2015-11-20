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

#include "engine.h"
#include "graphics/gl_include.h"

#include "texture.h"
#include <string>

namespace GTE
{
	//forward declarations
	class TextureGL;
	class RawImage;

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
}

#endif
