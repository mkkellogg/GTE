#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

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

#include <string>
#include "object/enginetypes.h"
#include "base/intmask.h"

enum class BlendingProperty
{
	One,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha
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

    Graphics();
    virtual ~Graphics();

    virtual bool Run();
    virtual void Update();
    virtual void PreProcessScene();
    virtual void RenderScene();

    public :

    RenderManager * GetRenderManager();

    virtual bool Init(const GraphicsAttributes& attributes);

    virtual Shader * CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) = 0;
    virtual void DestroyShader(Shader * shader) = 0;
    virtual SubMesh3DRenderer * CreateMeshRenderer(AttributeTransformer * attrTransformer)  = 0;
    virtual void DestroyMeshRenderer(SubMesh3DRenderer * buffer) = 0;
    virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
    virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
    virtual Texture * CreateTexture(const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual Texture * CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual void DestroyTexture(Texture * texture) = 0;
    virtual RenderTarget * CreateRenderTarget(IntMask buffers, unsigned int width, unsigned int height) = 0;
    virtual void DestroyRenderTarget(RenderTarget * target) = 0;

    virtual void ClearBuffers(unsigned int bufferMask) const = 0;

    virtual void EnableBlending(bool enabled) = 0;
    virtual void SetBlendingFunction(BlendingProperty source, BlendingProperty dest) = 0;
    virtual void ActivateMaterial(MaterialRef material);
    MaterialRef GetActiveMaterial() const;

    virtual const GraphicsAttributes& GetAttributes() const;
};

#endif
