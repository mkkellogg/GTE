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

#include <string>
#include "object/enginetypes.h"

class GraphicsCallbacks
{
    public:

    virtual void OnInit(Graphics * graphics) = 0;
    virtual void OnQuit(Graphics * graphics) = 0;
    virtual void OnUpdate(Graphics * graphics) = 0;
    virtual ~GraphicsCallbacks();
};

enum class BlendingProperty
{
	One,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha
};

class Graphics
{
	static Graphics * theInstance;

    protected:

	MaterialRef activeMaterial;
    RenderManager * renderManager;
    ScreenDescriptor * screenDescriptor;

    Graphics();
    virtual ~Graphics();

    public :

    static Graphics * Instance();

    RenderManager * GetRenderManager();

    virtual bool Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const std::string& windowTitle);
    virtual bool Run();

    virtual Shader * CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) = 0;
    virtual void DestroyShader(Shader * shader) = 0;
    virtual SubMesh3DRenderer * CreateMeshRenderer(AttributeTransformer * attrTransformer)  = 0;
    virtual void DestroyMeshRenderer(SubMesh3DRenderer * buffer) = 0;
    virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
    virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
    virtual Texture * CreateTexture(const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual Texture * CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual void DestroyTexture(Texture * texture) = 0;
    virtual void ClearBuffers(unsigned int bufferMask) const = 0;

    virtual void EnableBlending(bool enabled) = 0;
    virtual void SetBlendingFunction(BlendingProperty source, BlendingProperty dest) = 0;
    virtual void ActivateMaterial(MaterialRef material);
    MaterialRef GetActiveMaterial() const;

    virtual ScreenDescriptor * GetScreenDescriptor() const;
};

#endif
