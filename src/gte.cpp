
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics/graphics.h"
#include "graphics/graphicsGL.h"
#include "graphics/shader/shader.h"
#include "ui/debug.h"
#include "gte.h"

class CustomGraphicsCallbacks : public GraphicsCallbacks
{
    public : 

    CustomGraphicsCallbacks(){}

    void OnInit()
    {
        Shader basicShader("resources/basic.vertex.shader","resources/basic.fragment.shader");
        basicShader.Load();
    }
};


int main(int argc, char** argv)
{
    CustomGraphicsCallbacks graphicsCallbacks;
    GraphicsGL::Instance()->Init(640,480, (CustomGraphicsCallbacks*)(&graphicsCallbacks));   

    return EXIT_SUCCESS;
}
