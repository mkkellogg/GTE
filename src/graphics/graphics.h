#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

//forward declarations
class ViewSystem;
class Shader;
class Material;
class Mesh3DRenderer;
class RenderManager;
class Camera;
class Graphics;

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
    ViewSystem * viewSystem;
    RenderManager * renderManager;

    Graphics();
    virtual ~Graphics();

    public :

    static Graphics * Instance();

    virtual  void SendStandardUniformsToShader(const Camera * camera) = 0;
    RenderManager * GetRenderManager();

    virtual Material * CreateMaterial();
    virtual void DestroyMaterial(Material * material);

    virtual void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);

    virtual Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath) = 0;
    virtual void DestroyShader(Shader * shader) = 0;

    virtual Mesh3DRenderer * CreateMeshRenderer()  = 0;
    virtual void DestroyMeshRenderer(Mesh3DRenderer * buffer) = 0;

    virtual void ActivateMaterial(Material * material);
    Material * GetActiveMaterial() const;
};

#endif
