#include <math.h>
#include "rendertarget.h"
#include "base/intmask.h"
#include "global/global.h"

namespace GTE
{
	/*
	* Single constructor, set all member variables.
	*/
	RenderTarget::RenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer, const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height)
	{
		this->hasColorBuffer = hasColor;
		this->hasDepthBuffer = hasDepth;
		this->enableStencilBuffer = enableStencilBuffer;
		this->width = width;
		this->height = height;
		this->colorTextureAttributes = colorTextureAttributes;
	}


	/*
	 * Clean-up.
	 */
	RenderTarget::~RenderTarget()
	{

	}

	/*
	 * Return true if this render target supports [bufferType].
	 */
	bool RenderTarget::HasBuffer(RenderBufferType bufferType) const
	{
		switch (bufferType)
		{
		case RenderBufferType::Color:
			return hasColorBuffer;
			break;
		case RenderBufferType::Depth:
			return hasDepthBuffer;
			break;
		default:
			return false;
			break;
		}

		return false;
	}

	/*
	 * Get a reference to the depth texture.
	 */
	TextureRef RenderTarget::GetDepthTexture()
	{
		return depthTexture;
	}

	/*
	 * Get a reference to the color texture.
	 */
	TextureRef RenderTarget::GetColorTexture()
	{
		return colorTexture;
	}

	unsigned int RenderTarget::GetWidth()
	{
		return width;
	}

	unsigned int RenderTarget::GetHeight()
	{
		return height;
	}
}

