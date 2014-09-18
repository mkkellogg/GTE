#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//forward declarations
class ViewSystem;
class Shader;
class Material;
class Texture;
class Mesh3DRenderer;
class RenderManager;
class Camera;
class Graphics;
class SceneObject;
class Transform;
class ScreenDescriptor;
class VertexAttrBuffer;
class TextureAttributes;
class RawImage;

#include <string>

class GraphicsCallbacks
{
    public:

    virtual void OnInit(Graphics * graphics) = 0;
    virtual void OnQuit(Graphics * graphics) = 0;
    virtual void OnUpdate(Graphics * graphics) = 0;
    virtual ~GraphicsCallbacks();
};

class Graphics
{
	static Graphics * theInstance;

    protected:

    Material * activeMaterial;
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
    virtual Mesh3DRenderer * CreateMeshRenderer()  = 0;
    virtual void DestroyMeshRenderer(Mesh3DRenderer * buffer) = 0;
    virtual VertexAttrBuffer * CreateVertexAttributeBuffer() = 0;
    virtual void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) = 0;
    virtual Texture * CreateTexture(const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual Texture * CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes) = 0;
    virtual void DestroyTexture(Texture * texture) = 0;
    virtual void ClearBuffers(unsigned int bufferMask) const = 0;


    virtual void ActivateMaterial(Material * material);
    Material * GetActiveMaterial() const;

    virtual ScreenDescriptor * GetScreenDescriptor() const;
};

#endif
