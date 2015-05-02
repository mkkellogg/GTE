#include <math.h>
#include "graphics/gl_include.h"
#include "rendertargetGL.h"
#include "object/engineobjectmanager.h"
#include "base/intmask.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include "graphics/graphics.h"
#include "graphics/texture/texture.h"
#include "graphics/texture/textureGL.h"

/*
 * Single constructor. For the most part acts as a pass-through to the base constructor, but
 * also initializes member variables.
 */
RenderTargetGL::RenderTargetGL(bool hasColor, bool hasDepth,bool enableStencilBuffer,
							  const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height) :
							  RenderTarget(hasColor, hasDepth, enableStencilBuffer, colorTextureAttributes, width, height)
{
	fboID = 0;
}

/*
 * Clean-up.
 */
RenderTargetGL::~RenderTargetGL()
{
	Destroy();
}

/*
 * Destroy the FBO associated with this render target and all attached textures and/or
 * render buffers.
 */
void RenderTargetGL::Destroy()
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// destroy the color texture attachment
	if(colorTexture.IsValid())
	{
		TextureGL * texGL = dynamic_cast<TextureGL*>(colorTexture.GetPtr());
		if(texGL != NULL)
		{
			objectManager->DestroyTexture(colorTexture);
		}
		else
		{
			Debug::PrintError("RenderTargetGL::Destroy -> Unable to cast color texture to TextureGL.");
		}
	}

	// destroy the depth texture attachment
	if(depthTexture.IsValid())
	{
		TextureGL * texGL = dynamic_cast<TextureGL*>(depthTexture.GetPtr());
		if(texGL != NULL)
		{
			objectManager->DestroyTexture(depthTexture);
		}
		else
		{
			Debug::PrintError("RenderTargetGL::Destroy -> Unable to cast depth texture to TextureGL.");
		}
	}

	// destroy the FBO
	if(fboID > 0)
	{
		glDeleteFramebuffersEXT(1, &fboID);
	}
}

/*
 * Perform all initialization for this render target. This render target will not
 * be valid until this method successfully completes.
 */
bool RenderTargetGL::Init()
{
	// make sure to clean up existing frame-buffer objects (if they exist).
	Destroy();

	// generate an OpenGL FBO.
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	ASSERT(fboID != 0, "RenderTargetGL::Init -> Unable to create frame buffer object.");

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// generate a color texture attachment
	if(hasColorBuffer)
	{
		TextureAttributes attributes = colorTextureAttributes;

		if(attributes.IsCube)
		{
			// create a cube texture for this render target
			colorTexture =  objectManager->CreateCubeTexture(NULL, width, height,
														     NULL, width, height,
															 NULL, width, height,
															 NULL, width, height,
															 NULL, width, height,
															 NULL, width, height);
		}
		else
		{
			// create a 2D texture for this render target
			colorTexture =  objectManager->CreateTexture(width, height, NULL, attributes);
		}
		NONFATAL_ASSERT_RTRN(colorTexture.IsValid(), "RenderTargetGL::Init -> Unable to create color texture.", false, true);

		TextureGL * texGL = dynamic_cast<TextureGL*>(colorTexture.GetPtr());
		ASSERT(texGL != NULL, "RenderTargetGL::Init -> Unable to cast color texture to TextureGL.");

		if(attributes.IsCube)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texGL->GetTextureID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texGL->GetTextureID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texGL->GetTextureID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texGL->GetTextureID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texGL->GetTextureID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texGL->GetTextureID(), 0);

		}
		else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texGL->GetTextureID(), 0);
	}

	// generate a depth texture attachment
	if(hasDepthBuffer && ! enableStencilBuffer)
	{
		TextureAttributes attributes;
		attributes.FilterMode = TextureFilter::Point;
		attributes.WrapMode = TextureWrap::Clamp;
		attributes.IsDepthTexture = true;

		depthTexture =  objectManager->CreateTexture(width, height, NULL, attributes);
		NONFATAL_ASSERT_RTRN(depthTexture.IsValid(), "RenderTargetGL::Init -> Unable to create depth texture.", false, true);

		TextureGL * texGL = dynamic_cast<TextureGL*>(depthTexture.GetPtr());
		ASSERT(texGL != NULL, "RenderTargetGL::Init -> Unable to cast depth texture to TextureGL.");

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texGL->GetTextureID(), 0);
	}
	else if (hasDepthBuffer && enableStencilBuffer)
	{
		/***
		 * In OpengL, if we want depth AND stencil abilities then they must both be render buffers and
		 * they must be shared.
		 ***/

		GLuint depthStencilRenderBufferID;
		glGenRenderbuffers(1, &depthStencilRenderBufferID);
		if(depthStencilRenderBufferID == 0)
		{
			Debug::PrintError("RenderTargetGL::Init -> Unable to create depth/stencil render buffer.");
			Destroy();
			return false;
		}

		glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRenderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		//Attach stencil buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRenderBufferID);

	}

	unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	NONFATAL_ASSERT_RTRN(status==GL_FRAMEBUFFER_COMPLETE, "RenderTargetGL::Init -> Framebuffer is incomplete!.", false, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

/*
 * Get the OpenGL FBO ID.
 */
GLuint RenderTargetGL::GetFBOID()
{
	return fboID;
}

