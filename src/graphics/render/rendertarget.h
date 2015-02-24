#ifndef _GTE_RENDER_TARGET_H_
#define _GTE_RENDER_TARGET_H_

#include "base/intmask.h"
#include "renderbuffer.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"

class RenderTarget : EngineObject
{
	friend class Graphics;

    protected:

	bool hasColorBuffer;
	bool hasDepthBuffer;
	unsigned int width;
	unsigned int height;
	TextureRef colorTexture;
	TextureRef depthTexture;

	RenderTarget(bool hasColor, bool hasDepth, unsigned int width, unsigned int height);
    virtual ~RenderTarget();

    public:

    virtual bool Init() = 0;
    bool HasBuffer(RenderBufferType bufferType) const;
    TextureRef GetDepthTexture();
    TextureRef GetColorTexture();
};

#endif
