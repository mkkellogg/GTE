#include "textureGL.h"
#include "graphics/gl_include.h"
#include "textureattr.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	TextureGL::TextureGL(TextureAttributes attributes, GLuint textureID) : Texture(attributes)
	{
		this->textureID = textureID;
	}

	TextureGL::~TextureGL()
	{

	}

	GLuint TextureGL::GetTextureID() const
	{
		return textureID;
	}
}
