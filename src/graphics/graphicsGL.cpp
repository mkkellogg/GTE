#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "graphicsGL.h"
#include "material.h"
#include "ui/debug.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "vertexattrbuffer.h"
#include "vertexattrbufferGL.h"
#include "object/mesh3Drenderer.h"
#include "object/mesh3DrendererGL.h"
#include "gte.h"

GraphicsGL * _thisInstance;
GraphicsCallbacks * _instanceCallbacks;
void _glutDisplayFunc();
void _glutIdleFunc();

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
       Debug::PrintError("GLUT_DISPLAY_MODE_POSSIBLE = false.\n");
       exit(1);
    }

    (void)glutCreateWindow(windowTitle);

    glewExperimental = GL_TRUE; 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        Debug::PrintMessage("Ready for OpenGL 2.0\n");
    else {	
    	Debug::PrintError("OpenGL 2.0 not supported\n");
        exit(1);
    }

    glutDisplayFunc(&_glutDisplayFunc);
    glutIdleFunc(&_glutIdleFunc);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);

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

GraphicsGL::GraphicsGL() : Graphics(), callbacks(NULL)
{

}

Shader * GraphicsGL::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
    //TODO: Add switch for different platforms; for now only support OpenGL
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    return shader;
}

void GraphicsGL::DestroyShader(Shader * shader)
{
    delete shader;
}

Mesh3DRenderer * GraphicsGL::CreateMeshRenderer() const
{
	 //TODO: Add switch for different platforms; for now only support OpenGL
	return new Mesh3DRendererGL();
}

void GraphicsGL::DestroyMeshRenderer(Mesh3DRenderer * renderer)  const
{
	delete renderer;
}

void GraphicsGL::ActivateMaterial(Material * material)
{
	// TODO: Change this to a proper comparison, and not just
	// a comparison of memory addresses
	if(Graphics::GetActiveMaterial() != material)
	{
		Graphics::ActivateMaterial(material);

		ShaderGL * shader = (ShaderGL *) material->GetShader();
		glUseProgram(shader->GetProgramID());
	}
}

void _glutDisplayFunc()
{
	_instanceCallbacks->OnUpdate(_thisInstance);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // your drawing code goes here

    glutSwapBuffers();
}

void _glutIdleFunc()
{
	 glutPostRedisplay();
}



