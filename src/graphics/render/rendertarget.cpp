#include <math.h>
#include "rendertarget.h"
#include "base/intmask.h"
#include "global/global.h"

RenderTarget::RenderTarget(IntMask bufferTypes, unsigned int width, unsigned int height)
{
	this->bufferTypes = bufferTypes;
	this->width = width;
	this->height = height;
}

RenderTarget::~RenderTarget()
{

}

bool RenderTarget::HasBuffer(RenderBufferType bufferType) const
{
	return IntMaskUtil::IsBitSetForMask(bufferTypes, (unsigned int)bufferType);
}

