#ifndef _RENDER_TARGET_GL_H_
#define _RENDER_TARGET_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include "rendertarget.h"
#include "base/intmask.h"

class RenderTargetGL : public RenderTarget
{
	friend class GraphicsGL;

    protected:

	GLuint fboID;
	GLuint depthRenderBufferID;
	GLuint stencilRenderBufferID;
	GLuint colorAttachment0;

	RenderTargetGL(IntMask bufferTypes, unsigned int width, unsigned int height);
    ~RenderTargetGL();

    void Destroy();

    public:

    bool Init();
};

#endif
