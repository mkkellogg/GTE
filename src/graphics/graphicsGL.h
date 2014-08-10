#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_HL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics.h"
#include "vertexattrbuffer.h"

class GraphicsGL : Graphics
{
    friend class Graphics;

    protected:

    GraphicsCallbacks * callbacks;
    GraphicsGL();
    ~GraphicsGL();

    public :

    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks);
    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
    void DestroyShader(Shader * shader);
    void DestroyVertexAttrBuffer(VertexAttrBuffer * buffer);
    VertexAttrBuffer * CreateVertexAttrBuffer();
};

#endif
