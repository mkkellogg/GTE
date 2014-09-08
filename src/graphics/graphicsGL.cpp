#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "graphicsGL.h"
#include "graphics/screendesc.h"
#include "render/material.h"
#include "ui/debug.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "texture/textureGL.h"
#include "texture/texture.h"
#include "texture/textureattr.h"
#include "render/rendermanager.h"
#include "render/vertexattrbuffer.h"
#include "render/vertexattrbufferGL.h"
#include "render/mesh3Drenderer.h"
#include "render/mesh3DrendererGL.h"
#include "render/renderbuffer.h"
#include "image/imageloader.h"
#include "image/rawimage.h"
#include "view/camera.h"
#include "base/intmask.h"
#include "object/sceneobject.h"
#include "geometry/transform.h"
#include "global/global.h"

GraphicsGL * _thisInstance;
GraphicsCallbacks * _instanceCallbacks;

bool GraphicsGL::Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle)
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
    if (glewIsSupported("GL_VERSION_3_0"))
    {
    	Debug::PrintMessage("Using OpenGL 3.0");
    	openGLVersion =3;
    }
    else if (glewIsSupported("GL_VERSION_2_0"))
    {
    	Debug::PrintMessage("Using OpenGL 2.0");
    	openGLVersion = 2;
    }
    else
    {
    	openGLVersion = 1;
    }

    if(openGLVersion <= 1)
    {
    	 Debug::PrintError("Requires OpenGL 2.0 or greater.");
    	 return false;
    }

	// call base method
	Graphics::Init(windowWidth, windowHeight, callbacks, windowTitle);

    screenDescriptor = new ScreenDescriptor(windowWidth, windowHeight);

    glutDisplayFunc(&_glutDisplayFunc);
    glutIdleFunc(&_glutIdleFunc);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if(callbacks != NULL)
    {
        callbacks->OnInit(this);
    }

    return true;
}

bool GraphicsGL::Run()
{
	Graphics::Run();

	glutMainLoop();

	if(callbacks != NULL)
	{
		callbacks->OnQuit(this);
	}

	return true;
}

GraphicsGL::~GraphicsGL() 
{
    SAFE_DELETE(screenDescriptor);
}

GraphicsGL::GraphicsGL() : Graphics(), callbacks(NULL)
{
	openGLVersion = 0;
}

Shader * GraphicsGL::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
	NULL_CHECK(vertexShaderPath, "GraphicsGL::CreateShader -> NULL vertexShaderPath", NULL);
	NULL_CHECK(fragmentShaderPath, "GraphicsGL::CreateShader -> NULL fragmentShaderPath", NULL);

    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    bool loadSuccess = shader->Load();
	if(!loadSuccess)
	{
		char msg[64];
		sprintf(msg, "GraphicsGL::CreateShader -> could not load shader: '%s' or '%s' ", vertexShaderPath, fragmentShaderPath);
		Debug::PrintError(msg);
		return NULL;
    }
    return shader;
}

void GraphicsGL::DestroyShader(Shader * shader)
{
	NULL_CHECK_RTRN(shader, "GraphicsGL::DestroyShader -> shader is NULL");
    delete shader;
}

void GraphicsGL::ClearBuffers(unsigned int bufferMask) const
{
	GLbitfield glClearMask = 0;
	if(IntMask::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Color))
		glClearMask |= GL_COLOR_BUFFER_BIT;
	if(IntMask::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Depth))
		glClearMask |= GL_DEPTH_BUFFER_BIT;

	glClear(glClearMask);
}

Mesh3DRenderer * GraphicsGL::CreateMeshRenderer()
{
	return new Mesh3DRendererGL(this);
}

void GraphicsGL::DestroyMeshRenderer(Mesh3DRenderer * renderer)
{
	NULL_CHECK_RTRN(renderer, "GraphicsGL::DestroyMeshRenderer -> renderer is NULL");
	delete renderer;
}

VertexAttrBuffer * GraphicsGL::CreateVertexAttributeBuffer()
{
	return new VertexAttrBufferGL();
}

void GraphicsGL::DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer)
{
	NULL_CHECK_RTRN(buffer, "GraphicsGL::DestroyVertexAttributeBuffer -> buffer is NULL");
	delete buffer;
}

Texture * GraphicsGL::CreateTexture(const char * sourcePath, TextureAttributes attributes)
{
	NULL_CHECK(sourcePath, "GraphicsGL::CreateTexture -> sourcePath is NULL", NULL);

	RawImage * raw = ImageLoader::LoadPNG(sourcePath);

	NULL_CHECK(raw, "GraphicsGL::CreateTexture -> unable to create raw image", NULL);

	glEnable(GL_TEXTURE_2D);
	GLuint tex;
	glGenTextures(1, &tex);

	if(tex == 0)
	{
		Debug::PrintError("GraphicsGL::CreateTexture -> unable to gen texture");
		return NULL;
	}

	glBindTexture(GL_TEXTURE_2D, tex);

	if(attributes.WrapMode == TextureWrap::Mirror)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	else if(attributes.WrapMode == TextureWrap::Repeat)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	if(attributes.FilterMode == TextureFilter::Point)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if(attributes.FilterMode == TextureFilter::BiLinear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if(attributes.FilterMode == TextureFilter::TriLinear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw->GetWidth(), raw->GetHeight(), 0, GL_RGBA, GL_FLOAT, raw->GetPixels());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw->GetWidth(), raw->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, raw->GetPixels());

	if(openGLVersion >= 3)
	{
		glTexStorage2D(GL_TEXTURE_2D, attributes.MipMapLevel, GL_RGBA8, raw->GetWidth(), raw->GetHeight());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, raw->GetWidth(), raw->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, raw->GetPixels());
		if(attributes.FilterMode == TextureFilter::TriLinear)glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, attributes.MipMapLevel);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw->GetWidth(), raw->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, raw->GetPixels());
	}


	TextureGL * texture = new TextureGL(attributes, tex, sourcePath);
	return texture;
}

void GraphicsGL::DestroyTexture(Texture * texture)
{
	delete texture;
}

void GraphicsGL::ActivateMaterial(Material * material)
{
	NULL_CHECK_RTRN(material,"GraphicsGL::ActivateMaterial -> material is NULL");

	// TODO: Change this to a proper comparison, and not just
	// a comparison of memory addresses
	if(Graphics::GetActiveMaterial() != material)
	{
		Graphics::ActivateMaterial(material);

		Shader * shader = material->GetShader();

		NULL_CHECK_RTRN(shader,"GraphicsGL::ActivateMaterial -> shader is NULL");

		ShaderGL * shaderGL = dynamic_cast<ShaderGL *>(shader);

		NULL_CHECK_RTRN(shaderGL,"GraphicsGL::ActivateMaterial -> material's shader is not ShaderGL !!");

		glUseProgram(shaderGL->GetProgramID());
	}
}

void GraphicsGL::RenderScene()
{
	renderManager->RenderAll();

	glutSwapBuffers();
}

unsigned int GraphicsGL::GetOpenGLVersion()
{
	return openGLVersion;
}

void GraphicsGL::_glutDisplayFunc()
{
	_instanceCallbacks->OnUpdate(_thisInstance);

	_thisInstance->RenderScene();
}

void GraphicsGL::_glutIdleFunc()
{
	 glutPostRedisplay();
}



