#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "textureGL.h"
#include "textureattr.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <GL/glew.h>
#include <GL/glut.h>

TextureGL::TextureGL(TextureAttributes attributes, GLuint textureID) : Texture(attributes)
{
	this->textureID = textureID;
}

TextureGL::TextureGL(TextureAttributes attributes, GLuint textureID, RawImage* imageData) : Texture(attributes, imageData)
{
	this->textureID = textureID;
}

TextureGL::TextureGL(TextureAttributes attributes, GLuint textureID,  std::vector<RawImage *>& imageData) : Texture(attributes, imageData)
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
