
/*
* class: Graphics
*
* author: Mark Kellogg
*
* Graphics system
*
* Base class that defines the core graphics functionality of the engine,
* and acts as the interface to the lower-level graphics capabilities of engine.
* All platform-specific graphics functionality should be put in a class
* that derives from Graphics.
*
* All graphics related objects (shaders, textures, render targets, etc.)
* are created via functions contained in this class. This is due to the fact
* that creating these kinds of objects will require platform specific code,
* that is why the relevant methods are virtual and must be implemented in a
* deriving class.
*
* Additionally this class contains methods for setting various state variables.
* Again this is something that is platform specific, so the methods are
* virtual.
*/

#ifndef _GTE_GRAPHICS_H_
#define _GTE_GRAPHICS_H_

#include <string>

#include "engine.h"
#include "graphicsattr.h"
#include "renderstate.h"
#include "base/bitmask.h"
#include "render/rendertarget.h"
#include "render/material.h"
#include "global/global.h"

namespace GTE {
    //forward declarations
    class ViewSystem;
    class Shader;
    class Material;
    class Texture;
    class Atlas;
    class SubMesh3DRenderer;
    class RenderManager;
    class Camera;
    class Graphics;
    class SceneObject;
    class Transform;
    class ScreenDescriptor;
    class VertexAttrBuffer;
    class TextureAttributes;
    class RawImage;
    class AttributeTransformer;
    class RenderTarget;
    class ShaderSource;

    enum GraphicsError {
        InvalidRenderTarget = 1
    };

    class Graphics {
        // necessary to trigger life-cycle events
        friend class Engine;
        // necessary so that the EngineObjectManager can create objects like textures and render targets
        // that it needs to manager
        friend class EngineObjectManager;
        // necessary so that the ForwardRenderManager can activate materials and send material uniforms and
        // attributes to the active shader
        friend class ForwardRenderManager;
        // necessary so that SubMesh3DRenderer can directly call rendering methods
        friend class SubMesh3DRenderer;
        // the Camera class needs to be able to access information about the default & active render targets
        friend class Camera;

    protected:

        // descriptor that describes the properties with which the graphics
        // system was initialized
        GraphicsAttributes attributes;
        // length of current span over which FPS is calculated
        Real currentFPSSpanTime;
        // number of frames in rendered in current FPS calculation span
        Int32 framesInFPSSpan;
        // last calculated FPS value
        Real currentFPS;

        Graphics();
        virtual ~Graphics();

        virtual Bool Start();
        virtual void End();
        virtual void Update();
        virtual void PostRender();

        void UpdateFPS();

        virtual Bool Init(const GraphicsAttributes& attributes);
        virtual RenderTarget * CreateDefaultRenderTarget() = 0;
        RenderTargetSharedPtr SetupDefaultRenderTarget();
        virtual Bool ActivateRenderTarget(RenderTargetRef target) = 0;
        virtual RenderTargetRef GetCurrrentRenderTarget() = 0;
        virtual Bool ActivateCubeRenderTargetSide(CubeTextureSide side) = 0;
        virtual Bool RestoreDefaultRenderTarget() = 0;

        virtual Shader * CreateShader(const ShaderSource& shaderSource) = 0;
        virtual void DestroyShader(Shader * shader) = 0;
        virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
        virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
        virtual Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes& attributes) = 0;
        virtual Texture * CreateTexture(RawImage * imageData, const TextureAttributes& attributes) = 0;
        virtual Texture * CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, const TextureAttributes& attributes) = 0;
        virtual Texture * CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
                                            Byte * backData, UInt32 backw, UInt32 backh,
                                            Byte * topData, UInt32 tw, UInt32 th,
                                            Byte * bottomData, UInt32 botw, UInt32 both,
                                            Byte * leftData, UInt32 lw, UInt32 lh,
                                            Byte * rightData, UInt32 rw, UInt32 rh) = 0;
        virtual Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
                                            const std::string& bottom, const std::string& left, const std::string& right) = 0;
        virtual Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
                                            RawImage * bottomData, RawImage * leftData, RawImage * rightData) = 0;
        virtual void DestroyTexture(Texture * texture) = 0;
        virtual RenderTarget * CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
                                                  const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height) = 0;
        virtual void DestroyRenderTarget(RenderTarget * target) = 0;
        virtual RenderTargetRef GetDefaultRenderTarget() = 0;

        virtual void ActivateMaterial(MaterialRef, Bool reverseFaceCulling) = 0;
        virtual MaterialRef GetActiveMaterial() = 0;

        virtual void EnterRenderMode(RenderMode renderMode) = 0;

        virtual void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundAttributeBuffers, UInt32 vertexCount, Bool validate) = 0;

    public:



        Real GetCurrentFPS() const;

        virtual void ClearRenderBuffers(IntMask bufferMask) const = 0;

        virtual void SetFaceCullingMode(RenderState::FaceCulling mode) = 0;
        virtual RenderState::FaceCulling GetFaceCullingMode() const = 0;

        virtual void SetColorBufferChannelState(Bool r, Bool g, Bool b, Bool a) = 0;

        virtual void SetDepthBufferEnabled(Bool enabled) = 0;
        virtual void SetDepthBufferReadOnly(Bool readOnly) = 0;
        virtual void SetDepthBufferFunction(RenderState::DepthBufferFunction function) = 0;

        virtual void SetStencilBufferEnabled(Bool enabled) = 0;
        virtual void SetStencilTestEnabled(Bool enabled) = 0;

        virtual void SetFaceCullingEnabled(Bool enabled) = 0;

        virtual void SetBlendingEnabled(Bool enabled) = 0;
        virtual void SetBlendingFunction(RenderState::BlendingMethod source, RenderState::BlendingMethod dest) = 0;

        virtual const GraphicsAttributes& GetAttributes() const;

        virtual void CopyBetweenRenderTargets(RenderTargetRef src, RenderTargetConstRef dest) const = 0;

        virtual void SetTextureData(TextureRef texture, const Byte * data) const = 0;
        virtual void SetTextureData(TextureRef texture, const Byte * data, CubeTextureSide side) const = 0;
        virtual void RebuildMipMaps(TextureRef texture) const = 0;

        virtual Bool AddClipPlane() = 0;
        virtual void DeactiveAllClipPlanes() = 0;
    };
}

#endif
