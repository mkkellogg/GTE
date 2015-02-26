#ifndef _GTE_RENDER_TARGET_H_
#define _GTE_RENDER_TARGET_H_

// forward declarations
class TextureAttributes;

#include "base/intmask.h"
#include "renderbuffer.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "graphics/texture/textureattr.h"

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
	TextureAttributes colorTextureAttributes;

	RenderTarget(bool hasColor, bool hasDepth, const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height);
    virtual ~RenderTarget();

    public:

    virtual bool Init() = 0;
    bool HasBuffer(RenderBufferType bufferType) const;
    TextureRef GetDepthTexture();
    TextureRef GetColorTexture();
};

#endif
