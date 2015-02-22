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
#include "object/enginetypes.h"
#include "base/intmask.h"
#include "render/rendertarget.h"
#include "render/material.h"
#include "global/global.h"

enum class BlendingProperty
{
	One,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	Zero
};

enum class ColorChannel
{
	Red,
	Green,
	Blue,
	Alpha
};

enum class RenderMode
{
	Standard = 1,
	StandardWithShadowVolumeTest = 2,
	ShadowVolumeRender = 3,
	DepthOnly = 4,
	None = 0
};

class GraphicsAttributes
{
	public:

	unsigned int WindowWidth;
	unsigned int WindowHeight;
	std::string WindowTitle;
};

class Graphics
{
	friend class Engine;

    protected:

	MaterialRef activeMaterial;
    RenderManager * renderManager;
    GraphicsAttributes attributes;

    float fpsTime;
    int frames;
    float currentFPS;

    Graphics();
    virtual ~Graphics();

    virtual bool Run();
    virtual void Update();
    virtual void PreProcessScene();
    virtual void RenderScene();

    void UpdateFPS();

    public :

    RenderManager * GetRenderManager();
    float GetCurrentFPS();

    virtual bool Init(const GraphicsAttributes& attributes);

    virtual Shader * CreateShader(const ShaderSource& shaderSource) = 0;
    virtual void DestroyShader(Shader * shader) = 0;
    virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
    virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
    virtual Texture * CreateTexture(const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual Texture * CreateTexture(RawImage * imageData,  TextureAttributes attributes) = 0;
    virtual Texture * CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, TextureAttributes attributes) = 0 ;
    virtual Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
		    						    const std::string& bottom, const std::string& left, const std::string& right) = 0;
    virtual Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
										RawImage * bottomData, RawImage * leftData, RawImage * rightData) = 0;
    virtual void DestroyTexture(Texture * texture) = 0;
    virtual RenderTarget * CreateRenderTarget(bool hasColor, bool hasDepth, unsigned int width, unsigned int height) = 0;
    virtual void DestroyRenderTarget(RenderTarget * target) = 0;

    virtual void ClearRenderBuffers(unsigned int bufferMask) = 0;

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

    virtual bool ActivateRenderTarget(RenderTarget * target) = 0;
    virtual bool RestoreDefaultRenderTarget() = 0;

    virtual void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, unsigned int vertexCount, bool validate) = 0;

};

#endif
