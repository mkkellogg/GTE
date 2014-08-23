#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics.h"
#include "render/mesh3Drenderer.h"
#include "render/material.h"
#include "shader/shader.h"
#include "geometry/transform.h"

class GraphicsGL : public Graphics
{
    friend class Graphics;

    protected:

    GraphicsCallbacks * callbacks;
    GraphicsGL();
    ~GraphicsGL();

    void SendStandardUniformsToShader(const Camera * camera);

    public :

    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);

    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
    void DestroyShader(Shader * shader);

    Mesh3DRenderer * CreateMeshRenderer() const;
    void DestroyMeshRenderer(Mesh3DRenderer * buffer) const;

    void ActivateMaterial(Material * material);
    Material * GetActiveMaterial();

    void RenderScene();
    void RenderSceneObjects(const Camera * camera);
    void ClearBuffersForCamera(const Camera * camera) const;
};

#endif
