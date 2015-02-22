#include <math.h>
#include "rendertarget.h"
#include "base/intmask.h"
#include "global/global.h"

RenderTarget::RenderTarget(bool hasColor, bool hasDepth, unsigned int width, unsigned int height)
{
	this->hasColorBuffer = hasColor;
	this->hasDepthBuffer = hasDepth;
	this->width = width;
	this->height = height;
}

RenderTarget::~RenderTarget()
{

}

bool RenderTarget::HasBuffer(RenderBufferType bufferType) const
{
	switch(bufferType)
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

