
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

//forward declarations
class ViewSystem;
class Shader;
class Material;
class Texture;
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

#include <string>
#include "graphicsattr.h"
#include "object/enginetypes.h"
#include "base/intmask.h"
#include "render/rendertarget.h"
#include "render/material.h"
#include "global/global.h"

enum GraphicsError
{
	InvalidRenderTarget = 1
};

class Graphics
{
	friend class Engine;
	friend class EngineObjectManager;

    protected:

	// the material that is currently being used for rendering
	MaterialRef activeMaterial;
	// descriptor that describes the properties with which the graphics
	// system was initialized
    GraphicsAttributes attributes;

    // length of current span over which FPS is calculated
    float currentFPSSpanTime;
    // number of frames in rendered in current FPS calculation span
    int framesInFPSSpan;
    // last calculated FPS value
    float currentFPS;

    Graphics();
    virtual ~Graphics();

    virtual bool Start();
    virtual void End();
    virtual void Update();
    virtual void PreProcessScene();
    virtual void RenderScene();

    void UpdateFPS();

    virtual bool Init(const GraphicsAttributes& attributes);
    virtual RenderTarget * CreateDefaultRenderTarget() = 0;
    RenderTargetRef SetupDefaultRenderTarget();

    public :

    virtual Shader * CreateShader(const ShaderSource& shaderSource) = 0;
    virtual void DestroyShader(Shader * shader) = 0;
    virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
    virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
    virtual Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes& attributes) = 0;
    virtual Texture * CreateTexture(RawImage * imageData,  const TextureAttributes& attributes) = 0;
    virtual Texture * CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, const TextureAttributes& attributes) = 0 ;
    virtual Texture * CreateCubeTexture(BYTE * frontData, unsigned int fw, unsigned int fh,
										    BYTE * backData, unsigned int backw, unsigned int backh,
										    BYTE * topData, unsigned int tw, unsigned int th,
										    BYTE * bottomData, unsigned int botw, unsigned int both,
										    BYTE * leftData, unsigned int lw, unsigned int lh,
										    BYTE * rightData, unsigned int rw, unsigned int rh) = 0;
    virtual Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
									const std::string& bottom, const std::string& left, const std::string& right) = 0;
    virtual Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
									RawImage * bottomData, RawImage * leftData, RawImage * rightData) = 0;
    virtual void DestroyTexture(Texture * texture) = 0;
    virtual RenderTarget * CreateRenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer,
		   	   	   	   	   	   	   	   	     const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height) = 0;
    virtual void DestroyRenderTarget(RenderTarget * target) = 0;
    virtual RenderTargetRef GetDefaultRenderTarget() = 0;

    float GetCurrentFPS();

    virtual void ClearRenderBuffers(unsigned int bufferMask) = 0;

    virtual void SetFaceCullingMode(FaceCullingMode mode) = 0;
    virtual FaceCullingMode GetFaceCullingMode() = 0;

    virtual void SetColorBufferChannelState(bool r, bool g, bool b, bool a) = 0;

    virtual void SetDepthBufferEnabled(bool enabled) = 0;
    virtual void SetDepthBufferReadOnly(bool readOnly) = 0;
    virtual void SetDepthBufferFunction(DepthBufferFunction function) = 0;

    virtual void SetStencilBufferEnabled(bool enabled) = 0;
    virtual void SetStencilTestEnabled(bool enabled) = 0;

    virtual void SetFaceCullingEnabled(bool enabled) = 0;

    virtual void SetBlendingEnabled(bool enabled) = 0;
    virtual void SetBlendingFunction(BlendingProperty source, BlendingProperty dest) = 0;

    virtual void ActivateMaterial(MaterialRef material);
    MaterialRef GetActiveMaterial() const;

    virtual const GraphicsAttributes& GetAttributes() const;

    virtual void EnterRenderMode(RenderMode renderMode) = 0;

    virtual bool ActivateRenderTarget(RenderTargetRef target) = 0;
    virtual RenderTargetRef GetCurrrentRenderTarget() = 0;
    virtual bool ActivateCubeRenderTargetSide(CubeTextureSide side) = 0;
    virtual bool RestoreDefaultRenderTarget() = 0;
    virtual void CopyBetweenRenderTargets(RenderTargetRef src, RenderTargetRef dest) = 0;

    virtual void SetTextureData(TextureRef texture, BYTE * data) = 0;
    virtual void SetTextureData(TextureRef texture, BYTE * data, CubeTextureSide side) = 0;
    virtual void RebuildMipMaps(TextureRef texture) = 0;

    virtual bool AddClipPlane() = 0;
    virtual void DeactiveAllClipPlanes() = 0;

    virtual void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, unsigned int vertexCount, bool validate) = 0;

};

#endif
