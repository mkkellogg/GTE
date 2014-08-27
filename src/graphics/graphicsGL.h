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

#include "graphics.h"


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

    void SendStandardUniformsToShader(const Transform * modelView, const Transform * projection);


    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
    void DestroyShader(Shader * shader);
    Mesh3DRenderer * CreateMeshRenderer();
    void DestroyMeshRenderer(Mesh3DRenderer * buffer);


    void ActivateMaterial(Material * material);
    Material * GetActiveMaterial();

    void RenderScene();

    public :

    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);

};

#endif
