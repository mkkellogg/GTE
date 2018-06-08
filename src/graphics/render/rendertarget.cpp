#include "rendertarget.h"
#include "base/bitmask.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE {
    /*
    * Single constructor, set all member variables.
    */
    RenderTarget::RenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer, const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height) {
        this->hasColorBuffer = hasColor;
        this->hasDepthBuffer = hasDepth;
        this->enableStencilBuffer = enableStencilBuffer;
        this->width = width;
        this->height = height;
        this->colorTextureAttributes = colorTextureAttributes;

        depthBufferIsTexture = false;
        colorBufferIsTexture = false;
    }


    /*
     * Clean-up.
     */
    RenderTarget::~RenderTarget() {

    }

    /*
     * Return true if this render target supports [bufferType].
     */
    Bool RenderTarget::HasBuffer(RenderBufferType bufferType) const {
        switch (bufferType) {
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
    TextureRef RenderTarget::GetDepthTexture() {
        return depthTexture;
    }

    /*
     * Get a reference to the color texture.
     */
    TextureRef RenderTarget::GetColorTexture() {
        return colorTexture;
    }

    UInt32 RenderTarget::GetWidth() const {
        return width;
    }

    UInt32 RenderTarget::GetHeight() const {
        return height;
    }

    Bool RenderTarget::IsColorBufferTexture() const {
        return colorBufferIsTexture;
    }

    Bool RenderTarget::IsDepthBufferTexture() const {
        return depthBufferIsTexture;
    }
}

