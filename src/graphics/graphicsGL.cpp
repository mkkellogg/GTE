#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "graphicsGL.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "vertexattrbuffer.h"
#include "vertexattrbufferGL.h"
#include "gte.h"

void GraphicsGL::Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks)
{
    this->callbacks = callbacks;

    int argc = 0;
    char * argv = (char*)"";
    glutInit(&argc, &argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);

    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       exit(1);
    }

    (void)glutCreateWindow("GLUT Program");

    glewExperimental = GL_TRUE; 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {	
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }

    if(callbacks != NULL)
    {
        callbacks->OnInit(this);
    }

    glutMainLoop(); 

    if(callbacks != NULL)
    {
        callbacks->OnQuit(this);
    }
}

GraphicsGL::~GraphicsGL() 
{
    
}

GraphicsGL::GraphicsGL() : Graphics()
{
    
}

Shader * GraphicsGL::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
    //TODO: Add switch for different platforms; for now only support OpenGL
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    return shader;
}

VertexAttrBuffer * GraphicsGL::CreateVertexAttrBuffer()
{
    VertexAttrBufferGL * buffer = new VertexAttrBufferGL();
    return buffer;
}

void GraphicsGL::DestroyShader(Shader * shader)
{
    delete shader;
}

void GraphicsGL::DestroyVertexAttrBuffer(VertexAttrBuffer * buffer)
{
    delete buffer;
}



