#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_HL_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include "graphics.h"

class GraphicsGL : Graphics
{
    GraphicsCallbacks * callbacks;
    static GraphicsGL * theInstance;
    GraphicsGL();

    public :

    static GraphicsGL * Instance();
    ~GraphicsGL();
    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks);
};

#endif
