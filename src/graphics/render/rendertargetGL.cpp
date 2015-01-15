#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "rendertargetGL.h"
#include "base/intmask.h"
#include "global/global.h"
#include "debug/debug.h"

RenderTargetGL::RenderTargetGL(IntMask bufferTypes, unsigned int width, unsigned int height) : RenderTarget(bufferTypes, width, height)
{
	fboID = 0;
	depthRenderBufferID = 0;
	stencilRenderBufferID = 0;
	colorAttachment0 = 0;
}

RenderTargetGL::~RenderTargetGL()
{
	Destroy();
}

void RenderTargetGL::Destroy()
{
	if(depthRenderBufferID >0)
	{
		glDeleteRenderbuffersEXT(1, &depthRenderBufferID);
		depthRenderBufferID = 0;
	}

	if(fboID > 0)
	{
		glDeleteFramebuffersEXT(1, &fboID);
	}

	if(colorAttachment0 >0)
	{
		glDeleteTextures(1, &colorAttachment0);
		colorAttachment0 = 0;
	}
}

bool RenderTargetGL::Init()
{
	Destroy();

	glGenFramebuffersEXT(1, &fboID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);

	ASSERT(fboID != 0, "RenderTargetGL::Init -> Unable to create frame buffer object.", false);

	//Attach 2D texture to this FBO
	glGenTextures(1, &colorAttachment0);
	if(colorAttachment0 == 0)
	{
		Debug::PrintError("RenderTargetGL::Init -> Unable to create color attachment.");
		Destroy();
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, colorAttachment0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorAttachment0, 0);

	if(IntMaskUtil::IsBitSet(bufferTypes, (int)RenderBufferType::Depth))
	{
		glGenRenderbuffersEXT(1, &depthRenderBufferID);
		if(depthRenderBufferID == 0)
		{
			Debug::PrintError("RenderTargetGL::Init -> Unable to create depth render buffer.");
			Destroy();
			return false;
		}

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBufferID);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);

		//Attach depth buffer to FBO
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBufferID);
	}

	if(IntMaskUtil::IsBitSet(bufferTypes, (int)RenderBufferType::Stencil))
	{
		glGenRenderbuffersEXT(1, &stencilRenderBufferID);
		if(depthRenderBufferID == 0)
		{
			Debug::PrintError("RenderTargetGL::Init -> Unable to create stencil render buffer.");
			Destroy();
			return false;
		}

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilRenderBufferID);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8_EXT, width, height);

		//Attach depth buffer to FBO
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilRenderBufferID);
	}

	return true;
}
