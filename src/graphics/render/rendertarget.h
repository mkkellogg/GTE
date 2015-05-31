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
		Bool hasColorBuffer;
		// does this render target support depth rendering?
		Bool hasDepthBuffer;
		// enable stencil buffer for render (but not as a render target)
		Bool enableStencilBuffer;
		// width of this render target
		UInt32 width;
		// height of this render target
		UInt32 height;
		// texture to which color rendering will occur
		TextureRef colorTexture;
		// texture to which depth rendering will occur
		TextureRef depthTexture;
		// texture attributes of [colorTexture]
		TextureAttributes colorTextureAttributes;

		RenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer, const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height);
		virtual ~RenderTarget();

	public:

		virtual Bool Init() = 0;
		Bool HasBuffer(RenderBufferType bufferType) const;
		TextureRef GetDepthTexture();
		TextureRef GetColorTexture();
		UInt32 GetWidth();
		UInt32 GetHeight();
	};
}

#endif
