/*
 * class:  RenderTarget
 *
 * Author: Mark Kellogg
 *
 * Base class for encapsulating a target for rendering. Platform specific
 * implementations are needed to complete this base implementation as the
 * concept of off-screen rendering is platform-dependent.
 */

#ifndef _GTE_RENDER_TARGET_H_
#define _GTE_RENDER_TARGET_H_

#include "base/intmask.h"
#include "renderbuffer.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "graphics/texture/textureattr.h"

namespace GTE
{
	// forward declarations
	class TextureAttributes;

	class RenderTarget : public EngineObject
	{
	protected:

		// does this render target support standard color-buffer rendering?
		bool hasColorBuffer;
		// does this render target support depth rendering?
		bool hasDepthBuffer;
		// enable stencil buffer for render (but not as a render target)
		bool enableStencilBuffer;
		// width of this render target
		unsigned int width;
		// height of this render target
		unsigned int height;
		// texture to which color rendering will occur
		TextureRef colorTexture;
		// texture to which depth rendering will occur
		TextureRef depthTexture;
		// texture attributes of [colorTexture]
		TextureAttributes colorTextureAttributes;

		RenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer, const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height);
		virtual ~RenderTarget();

	public:

		virtual bool Init() = 0;
		bool HasBuffer(RenderBufferType bufferType) const;
		TextureRef GetDepthTexture();
		TextureRef GetColorTexture();
		unsigned int GetWidth();
		unsigned int GetHeight();
	};
}

#endif
