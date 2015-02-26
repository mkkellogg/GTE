#ifndef _GTE_RENDER_TARGET_GL_H_
#define _GTE_RENDER_TARGET_GL_H_

// forward declarations
class TextureAttributes;

#include <GL/glew.h>
#include <GL/glut.h>
#include "rendertarget.h"
#include "base/intmask.h"

class RenderTargetGL : public RenderTarget
{
	friend class GraphicsGL;

    protected:

	GLuint fboID;

	RenderTargetGL(bool hasColor, bool hasDepth, const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height);
    ~RenderTargetGL();

    void Destroy();

    public:

    bool Init();
    GLuint GetFBOID();
};

#endif
