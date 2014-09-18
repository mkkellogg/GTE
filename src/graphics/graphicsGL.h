#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

//forward declarations
class Shader;
class Mesh3DRenderer;
class Mesh3DRendererGL;
class Material;
class Camera;
class VertexAttrBuffer;
class TextureAttributes;

#include "graphics.h"
#include <string>

class GraphicsGL : public Graphics
{
    friend class Graphics;
    friend class Mesh3DRendererGL;

    static void _glutDisplayFunc();
    static void _glutIdleFunc();

    protected:

    GraphicsCallbacks * callbacks;
    GraphicsGL();
    ~GraphicsGL();

    unsigned int openGLVersion;

    Shader * CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void DestroyShader(Shader * shader);
    Mesh3DRenderer * CreateMeshRenderer();
    void DestroyMeshRenderer(Mesh3DRenderer * buffer);
    VertexAttrBuffer * CreateVertexAttributeBuffer();
    void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
    Texture * CreateTexture(const std::string& sourcePath,  TextureAttributes attributes);
    Texture * CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes);
    void DestroyTexture(Texture * texture);
    void ClearBuffers(unsigned int bufferMask) const;

    void ActivateMaterial(Material * material);
    Material * GetActiveMaterial();

    void RenderScene();

    public :

    bool Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const std::string& windowTitle);
    bool Run();
    unsigned int GetOpenGLVersion();
};

#endif
