#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "graphics/gl_include.h"
#include "textureGL.h"
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
