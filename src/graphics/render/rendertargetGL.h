/*
 * class:  RenderTargetGL
 *
 * Author: Mark Kellogg
 *
 * OpenGL implementation of RenderTarget. Currently it makes use of Framebuffer Objects (FBO)
 * with a single texture color attachment at GL_COLOR_ATTACHMENT0, and single texture
 * depth attachment at GL_DEPTH_ATTACHMENT (as of now only one depth attachment is supported ).
 */

#ifndef _GTE_RENDER_TARGET_GL_H_
#define _GTE_RENDER_TARGET_GL_H_

#include "graphics/gl_include.h"
#include "object/enginetypes.h"
#include "rendertarget.h"
#include "base/intmask.h"

namespace GTE
{
	// forward declarations
	class TextureAttributes;

	class RenderTargetGL : public RenderTarget
	{
		friend class GraphicsGL;

		// OpenGL Framebuffer Object ID.
		GLuint fboID;

		RenderTargetGL(bool hasColor, bool hasDepth, bool enableStencilBuffer,
			const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height);
		~RenderTargetGL();

		void Destroy();

	public:

		bool Init();
		GLuint GetFBOID();
	};
}

#endif
