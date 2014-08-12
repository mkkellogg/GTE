#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics.h"
#include "vertexattrbuffer.h"

class GraphicsGL : public Graphics
{
    friend class Graphics;

    protected:

    GraphicsCallbacks * callbacks;
    GraphicsGL();
    ~GraphicsGL();

    public :

    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);
    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath) const;
    void DestroyShader(Shader * shader) const;
    VertexAttrBuffer * CreateVertexAttrBuffer() const;
    void DestroyVertexAttrBuffer(VertexAttrBuffer * buffer) const;
};

#endif
