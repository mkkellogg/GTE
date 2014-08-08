#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_HL_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include "graphics.h"

class GraphicsGL : Graphics
{
    friend class Graphics;

    GraphicsCallbacks * callbacks;
    GraphicsGL();

    public :

    ~GraphicsGL();
    void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks);
};

#endif
