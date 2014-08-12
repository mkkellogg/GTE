#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "graphicsGL.h"
#include "ui/debug.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "vertexattrbuffer.h"
#include "vertexattrbufferGL.h"
#include "gte.h"

GraphicsGL * _thisInstance;
GraphicsCallbacks * _instanceCallbacks;
void _glutDisplayFunc();

void GraphicsGL::Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle)
{
	_instanceCallbacks = this->callbacks = callbacks;
	_thisInstance = this;

    int argc = 0;
    char * argv = (char*)"";
    glutInit(&argc, &argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);

    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       exit(1);
    }

    (void)glutCreateWindow(windowTitle);

    glewExperimental = GL_TRUE; 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        Debug::PrintMessage("Ready for OpenGL 2.0\n");
    else {	
    	Debug::PrintMessage("OpenGL 2.0 not supported\n");
        exit(1);
    }

    glutDisplayFunc(&_glutDisplayFunc);

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

Shader * GraphicsGL::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath) const
{
    //TODO: Add switch for different platforms; for now only support OpenGL
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    return shader;
}

VertexAttrBuffer * GraphicsGL::CreateVertexAttrBuffer() const
{
    VertexAttrBufferGL * buffer = new VertexAttrBufferGL();
    return buffer;
}

void GraphicsGL::DestroyShader(Shader * shader) const
{
    delete shader;
}

void GraphicsGL::DestroyVertexAttrBuffer(VertexAttrBuffer * buffer) const
{
    delete buffer;
}

void _glutDisplayFunc()
{
	_instanceCallbacks->OnUpdate(_thisInstance);
}


