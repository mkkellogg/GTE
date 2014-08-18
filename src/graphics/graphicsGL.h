#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics.h"
#include "object/mesh3Drenderer.h"
#include "material.h"
#include "shader/shader.h"

class GraphicsGL : public Graphics
{
    friend class Graphics;

    protected:

    GraphicsCallbacks * callbacks;
    GraphicsGL();
    ~GraphicsGL();

    public :

    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);

    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
    void DestroyShader(Shader * shader);

    Mesh3DRenderer * CreateMeshRenderer() const;
    void DestroyMeshRenderer(Mesh3DRenderer * buffer) const;

    void ActivateMaterial(Material * material);
    Material * GetActiveMaterial();
};

#endif
