/*
 * class: TextureGL
 *
 * author: Mark Kellogg
 *
 * The OpenGL implementation of a texture.
 *
 */

#ifndef _TEXTURE_GL_H_
#define _TEXTURE_GL_H_

//forward declarations
class TextureGL;

#include "texture.h"
#include <GL/glew.h>
#include <GL/glut.h>

class TextureGL : public Texture
{
	friend class GraphicsGL;

	protected:

	GLuint textureID;

	TextureGL(TextureAttributes attributes, GLuint textureID);
	TextureGL(TextureAttributes attributes, GLuint textureID, const char * sourcePath);
	~TextureGL();

	public:

	GLuint GetTextureID() const;
};

#endif
