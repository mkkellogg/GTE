#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
 
#include "engine.h"
#include "graphicsGL.h"
#include "graphics/screendesc.h"
#include "graphics/animation/animationmanager.h"
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
#include "render/submesh3Drenderer.h"
#include "render/submesh3DrendererGL.h"
#include "render/rendertarget.h"
#include "render/rendertargetGL.h"
#include "render/attributetransformer.h"
#include "image/imageloader.h"
#include "image/rawimage.h"
#include "view/camera.h"
#include "base/intmask.h"
#include "object/sceneobject.h"
#include "geometry/transform.h"
#include "global/global.h"
#include "util/time.h"


//TODO: Right now, GLUT callbacks drive the engine loop. This is not ideal,
// so eventually we need to move the loop driver somewhere else

bool GraphicsGL::Init(const GraphicsAttributes& attributes)
{
	this->attributes = attributes;

    int argc = 0;
    char * argv = (char*)"";
    glutInit(&argc, &argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(this->attributes.WindowWidth, this->attributes.WindowHeight);

    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       Debug::PrintError("GLUT_DISPLAY_MODE_POSSIBLE = false.\n");
       exit(1);
    }

    (void)glutCreateWindow(this->attributes.WindowTitle.c_str());

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
	bool parentInit = Graphics::Init(this->attributes);
	if(!parentInit)return false;

    glutDisplayFunc(&_glutDisplayFunc);
    glutIdleFunc(&_glutIdleFunc);

    // TODO: think of a better place for these calls
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    blendingEnabled = false;

    return true;
}

GraphicsGL::~GraphicsGL() 
{

}

GraphicsGL::GraphicsGL() : Graphics()
{
	openGLVersion = 0;
}

Shader * GraphicsGL::CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    bool loadSuccess = shader->Load();
	if(!loadSuccess)
	{
		std::string msg = "GraphicsGL::CreateShader -> could not load shader: ";
		std::string singleQuote = std::string("'");
		msg += singleQuote + vertexShaderPath + singleQuote + std::string(" or ") + singleQuote +fragmentShaderPath + singleQuote;
		Debug::PrintError(msg);
		return NULL;
    }
    return shader;
}

void GraphicsGL::DestroyShader(Shader * shader)
{
	ASSERT_RTRN(shader != NULL, "GraphicsGL::DestroyShader -> shader is NULL");
    delete shader;
}

void GraphicsGL::ClearBuffers(unsigned int bufferMask) const
{
	GLbitfield glClearMask = 0;
	if(IntMaskUtil::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Color))
		glClearMask |= GL_COLOR_BUFFER_BIT;
	if(IntMaskUtil::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Depth))
		glClearMask |= GL_DEPTH_BUFFER_BIT;

	glClear(glClearMask);
}

SubMesh3DRenderer * GraphicsGL::CreateMeshRenderer(AttributeTransformer * attrTransformer)
{
	return new SubMesh3DRendererGL(this, attrTransformer);
}

void GraphicsGL::DestroyMeshRenderer(SubMesh3DRenderer * renderer)
{
	ASSERT_RTRN(renderer != NULL, "GraphicsGL::DestroyMeshRenderer -> renderer is NULL");

	SubMesh3DRendererGL * rendererGL = dynamic_cast<SubMesh3DRendererGL*>(renderer);
	if(rendererGL != NULL)
	{
		delete rendererGL;
	}
}

VertexAttrBuffer * GraphicsGL::CreateVertexAttributeBuffer()
{
	return new VertexAttrBufferGL();
}

void GraphicsGL::DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer)
{
	ASSERT_RTRN(buffer != NULL, "GraphicsGL::DestroyVertexAttributeBuffer -> buffer is NULL");
	delete buffer;
}

Texture * GraphicsGL::CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes)
{
	ASSERT(imageData != NULL, "GraphicsGL::CreateTexture -> imageData is NULL", NULL);

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

	RawImage * raw = const_cast<RawImage *>(imageData);

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

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );

	TextureGL * texture = new TextureGL(attributes, tex, sourcePath);
	return texture;
}

Texture * GraphicsGL::CreateTexture(const std::string& sourcePath, TextureAttributes attributes)
{
	RawImage * raw = ImageLoader::LoadImage(sourcePath);
	ASSERT(raw != NULL, "GraphicsGL::CreateTexture -> unable to create raw image", NULL);

	TextureGL * tex = (TextureGL*)CreateTexture(raw, sourcePath, attributes);
	if(tex == NULL)
	{
		Debug::PrintError("GraphicsGL::CreateTexture -> Unable to create texture.");
		ImageLoader::DestroyRawImage(raw);
		return NULL;
	}

	ImageLoader::DestroyRawImage(raw);
	return tex;
}

void GraphicsGL::DestroyTexture(Texture * texture)
{
	ASSERT_RTRN(texture != NULL, "GraphicsGL::DestroyTexture -> texture is NULL");

	TextureGL * texGL = dynamic_cast<TextureGL*>(texture);
	if(texGL != NULL)
	{
		delete texGL;
	}
}

RenderTarget * GraphicsGL::CreateRenderTarget(IntMask buffers, unsigned int width, unsigned int height)
{
	RenderTargetGL * buffer;
	buffer = new RenderTargetGL(buffers, width, height);
	ASSERT(buffer != NULL, "GraphicsGL::CreateRenderTarget -> unable to create render target", NULL);
	return buffer;
}

void GraphicsGL::DestroyRenderTarget(RenderTarget * target)
{
	ASSERT_RTRN(target != NULL, "GraphicsGL::DestroyRenderTarget -> target is NULL");

	RenderTargetGL * targetGL = dynamic_cast<RenderTargetGL*>(target);
	if(targetGL != NULL)
	{
		delete targetGL;
	}
}

void GraphicsGL::EnableBlending(bool enabled)
{
	if(blendingEnabled == enabled)return;
	if(enabled)glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	blendingEnabled = enabled;
}

void GraphicsGL::SetBlendingFunction(BlendingProperty source, BlendingProperty dest)
{
	glBlendFunc(GetGLBlendProperty(source),GetGLBlendProperty(dest));
}

GLenum GraphicsGL::GetGLBlendProperty(BlendingProperty property)
{
	switch(property)
	{
		case BlendingProperty::SrcAlpha:
			return GL_SRC_ALPHA;
		break;
		case BlendingProperty::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		break;
		case BlendingProperty::DstAlpha:
			return GL_DST_ALPHA;
		break;
		case BlendingProperty::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		break;
		case BlendingProperty::One:
			return GL_ONE;
		break;
		default:
			return (GLenum)0xFFFFFFFF;
		break;
	}

	return (GLenum)0xFFFFFFFF;
}

void GraphicsGL::ActivateMaterial(MaterialRef material)
{
	ASSERT_RTRN(material.IsValid(),"GraphicsGL::ActivateMaterial -> material is NULL");

	// TODO: Change this to a proper comparison, and not just
	// a comparison of memory addresses
	if(!(Graphics::GetActiveMaterial() == material))
	{
		GLuint oldActiveProgramID = (GLuint)0xFFFFFFF0;
		if(this->activeMaterial.IsValid())
		{
			ShaderRef currentShader = this->activeMaterial->GetShader();
			if(currentShader.IsValid())
			{
				ShaderGL * currentShaderGL = dynamic_cast<ShaderGL *>(currentShader.GetPtr());
				if(currentShaderGL != NULL)
				{
					oldActiveProgramID = currentShaderGL->GetProgramID();
				}
			}
		}

		Graphics::ActivateMaterial(material);

		ShaderRef shader = material->GetShader();
		ASSERT_RTRN(shader.IsValid(),"GraphicsGL::ActivateMaterial -> shader is NULL");

		ShaderGL * shaderGL = dynamic_cast<ShaderGL *>(shader.GetPtr());
		ASSERT_RTRN(shaderGL != NULL,"GraphicsGL::ActivateMaterial -> material's shader is not ShaderGL !!");

		// only active the new shader if it is different from the currently active one
		if(oldActiveProgramID != shaderGL->GetProgramID())
		{
			glUseProgram(shaderGL->GetProgramID());
		}
	}
}

bool GraphicsGL::Run()
{
	Graphics::Run();
	glutMainLoop();
	return true;
}

/*
 * For now, this method does nothing.
 */
void GraphicsGL::PreProcessScene()
{

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

void GraphicsGL::Update()
{

}

void GraphicsGL::_glutDisplayFunc()
{
	Engine::Instance()->Update();
}

void GraphicsGL::_glutIdleFunc()
{
	 glutPostRedisplay();
}



