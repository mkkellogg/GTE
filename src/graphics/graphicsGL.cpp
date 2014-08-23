#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "graphicsGL.h"
#include "render/material.h"
#include "ui/debug.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "render/vertexattrbuffer.h"
#include "render/vertexattrbufferGL.h"
#include "render/mesh3Drenderer.h"
#include "render/mesh3DrendererGL.h"
#include "view/camera.h"
#include "render/renderbuffer.h"
#include "base/intmask.h"

GraphicsGL * _thisInstance;
GraphicsCallbacks * _instanceCallbacks;
void _glutDisplayFunc();
void _glutIdleFunc();

void GraphicsGL::Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle)
{
	// case base method
	Graphics::Init(windowWidth, windowHeight, callbacks, windowTitle);

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

void GraphicsGL::SendStandardUniformsToShader(const Camera * camera)
{
	if(viewSystem == NULL)
	{
		Debug::PrintError("GraphicsGL::SendStandardUniformsToShader() -> viewSystem is NULL.");
		return;
	}

	if(activeMaterial != NULL)
	{
		int mvMatrixLoc = activeMaterial->GetUniformShaderVarLocation(Uniform::ModelViewMatrix);
		int mvpMatrixLoc = activeMaterial->GetUniformShaderVarLocation(Uniform::ModelViewProjectionMatrix);
		int projectionMatrixLoc = activeMaterial->GetUniformShaderVarLocation(Uniform::ProjectionMatrix);

		ShaderGL * shader = (ShaderGL *) activeMaterial->GetShader();
		if(shader != NULL)
		{
			const Transform * modelViewTransform = camera->GetModelViewTransform();
			const Transform * mvpTransform = camera->GetMVPTransform();
			const Transform * projectionTransform = camera->GetProjectionTransform();

			if(mvMatrixLoc >= 0)shader->SendUniformToShader(mvMatrixLoc, modelViewTransform->GetMatrix());
			if(mvpMatrixLoc >= 0)shader->SendUniformToShader(mvpMatrixLoc, mvpTransform->GetMatrix());
			if(projectionMatrixLoc >= 0)shader->SendUniformToShader(projectionMatrixLoc, projectionTransform->GetMatrix());
		}
		else
		{
			Debug::PrintError("GraphicsGL::SendStandardUniformsToShader() -> material contains NULL shader.");
		}
	}
	else
	{
		Debug::PrintError("GraphicsGL::SendStandardUniformsToShader() -> activeMaterial is NULL.");
	}
}

void GraphicsGL::ClearBuffersForCamera(const Camera * camera) const
{
	unsigned int clearBufferMask = camera->GetClearBufferMask();
	GLbitfield glClearMask = 0;
	if(IntMask::IsBitSetForMask(clearBufferMask, (unsigned int)RenderBufferType::Color))
		glClearMask |= GL_COLOR_BUFFER_BIT;
	if(IntMask::IsBitSetForMask(clearBufferMask, (unsigned int)RenderBufferType::Depth))
		glClearMask |= GL_DEPTH_BUFFER_BIT;

	glClear(glClearMask);
}

void GraphicsGL::RenderScene()
{
	for( int i = 0; i < viewSystem->CameraCount(); i++)
	{
		const Camera * camera = viewSystem->GetCamera(i);
		ClearBuffersForCamera(camera);
		RenderSceneObjects(camera);
	}

	glutSwapBuffers();
}

void GraphicsGL::RenderSceneObjects(const Camera * camera)
{
	SendStandardUniformsToShader(camera);
}

void _glutDisplayFunc()
{
	_instanceCallbacks->OnUpdate(_thisInstance);

	_thisInstance->RenderScene();
}

void _glutIdleFunc()
{
	 glutPostRedisplay();
}



