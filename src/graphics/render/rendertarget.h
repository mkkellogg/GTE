#ifndef _GTE_RENDER_TARGET_H_
#define _GTE_RENDER_TARGET_H_

#include "base/intmask.h"
#include "renderbuffer.h"

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
