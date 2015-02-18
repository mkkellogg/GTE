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

#include "texture.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

class TextureGL : public Texture
{
	friend class GraphicsGL;

	protected:

	GLuint textureID;

	TextureGL(TextureAttributes attributes, GLuint textureID);
	TextureGL(TextureAttributes attributes, GLuint textureID, const std::string& sourcePath);
	TextureGL(TextureAttributes attributes, GLuint textureID, const std::vector<std::string>& sourcePaths);
	~TextureGL();

	public:

	GLuint GetTextureID() const;
};

#endif
