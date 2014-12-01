#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

#include "base/intmask.h"

enum class RenderBufferType
{
	Color=1,
	Depth=2,
	Stencil=4
};

class RenderTarget
{
	friend class Graphics;

    protected:

	IntMask bufferTypes;
	unsigned int width;
	unsigned int height;

	RenderTarget(IntMask bufferTypes, unsigned int width, unsigned int height);
    virtual ~RenderTarget();

    public:

    virtual bool Init() = 0;
    bool HasBuffer(RenderBufferType bufferType) const;
};

#endif
