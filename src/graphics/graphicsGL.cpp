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
#include "debug/gtedebug.h"
#include "shader/shaderGL.h"
#include "shader/shader.h"
#include "texture/textureGL.h"
#include "texture/texture.h"
#include "texture/textureattr.h"
#include "render/rendermanager.h"
#include "render/vertexattrbuffer.h"
#include "render/vertexattrbufferGL.h"
#include "render/submesh3Drenderer.h"
#include "render/rendertarget.h"
#include "render/renderbuffer.h"
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
    glutReshapeFunc(&_glutReshapeFunc);

    // TODO: think of a better place for these initial calls

    glClearColor(0,0,0,0);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    SetBlendingEnabled(false);

    SetDepthBufferEnabled(true);
    SetDepthBufferReadOnly(false);
    SetDepthBufferFunction(DepthBufferFunction::Equal);

    SetStencilBufferEnabled(false);

    SetFaceCullingEnabled(true);

    glGetIntegerv(GL_RED_BITS, &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS, &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
   // printf("color buffer Bits: %d, %d, %d, %d\n", redBits, greenBits, blueBits, alphaBits);

    glGetIntegerv(GL_DEPTH_BITS, &depthBufferBits);
    //printf("depth buffer bits: %d\n", depthBufferBits);

    glGetIntegerv(GL_STENCIL_BITS, &stencilBufferBits);
   // printf("stencil buffer bits: %d\n", stencilBufferBits);

    initialized = true;
    return true;
}

GraphicsGL::~GraphicsGL() 
{

}

GraphicsGL::GraphicsGL() : Graphics()
{
	openGLVersion = 0;
	blendingEnabled = false;
	colorBufferEnabled = false;

	depthBufferEnabled = false;
	depthBufferReadOnly = false;

	stencilTestEnabled = false;
	stencilBufferEnabled = false;

	faceCullingEnabled = false;

	initialized = false;

	redBits = -1;
	greenBits = -1;
	blueBits = -1;
	alphaBits = -1;
	depthBufferBits = -1;
	stencilBufferBits = -1;
}

Shader * GraphicsGL::CreateShader(const ShaderSource& shaderSource)
{
    Shader * shader = new ShaderGL(shaderSource);
    bool loadSuccess = shader->Load();
	if(!loadSuccess)
	{
		std::string msg = "GraphicsGL::CreateShader -> could not load shader: ";
		msg += std::string(shaderSource.GetName());
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

void GraphicsGL::ClearRenderBuffers(unsigned int bufferMask)
{
	GLbitfield glClearMask = 0;
	if(IntMaskUtil::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Color))
	{
		glClearMask |= GL_COLOR_BUFFER_BIT;
	}
	if(IntMaskUtil::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Depth))
	{
		SetDepthBufferReadOnly(false);
		glClearMask |= GL_DEPTH_BUFFER_BIT;
	}
	if(IntMaskUtil::IsBitSetForMask(bufferMask, (unsigned int)RenderBufferType::Stencil))
	{
		glClearMask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(glClearMask);
}

void GraphicsGL::SetColorBufferChannelState(bool r, bool g, bool b, bool a)
{
	GLboolean red = r == true ? GL_TRUE : GL_FALSE;
	GLboolean green = g == true ? GL_TRUE : GL_FALSE;
	GLboolean blue = b == true ? GL_TRUE : GL_FALSE;
	GLboolean alpha = a == true ? GL_TRUE : GL_FALSE;
	glColorMask(red, green, blue, alpha);
}

void GraphicsGL::SetDepthBufferEnabled(bool enabled)
{
	if(depthBufferEnabled != enabled || !initialized)
	{
		if(enabled)glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
		depthBufferEnabled = enabled;
	}
}

void GraphicsGL::SetDepthBufferReadOnly(bool readOnly)
{
	if(depthBufferReadOnly != readOnly || !initialized)
	{
		if(readOnly)glDepthMask(GL_FALSE);
		else glDepthMask(GL_TRUE);
		depthBufferReadOnly = readOnly;
	}
}

void GraphicsGL::SetDepthBufferFunction(DepthBufferFunction function)
{
	switch(function)
	{
		case DepthBufferFunction::Always:
			glDepthFunc(GL_ALWAYS);
		break;
		case DepthBufferFunction::Greater:
			glDepthFunc(GL_GREATER);
		break;
		case DepthBufferFunction::GreaterThanOrEqual:
			glDepthFunc(GL_GEQUAL);
		break;
		case DepthBufferFunction::Less:
			glDepthFunc(GL_LESS);
		break;
		case DepthBufferFunction::LessThanOrEqual:
			glDepthFunc(GL_LEQUAL);
		break;
		case DepthBufferFunction::Equal:
			glDepthFunc(GL_EQUAL);
		break;
	}
}

void GraphicsGL::SetStencilBufferEnabled(bool enabled)
{
	if(stencilBufferEnabled != enabled || !initialized)
	{
		if(enabled)glEnable(GL_STENCIL_BUFFER);
		else glDisable(GL_STENCIL_BUFFER);
		stencilBufferEnabled = enabled;
	}
}

void GraphicsGL::SetStencilTestEnabled(bool enabled)
{
	if(stencilTestEnabled != enabled || !initialized)
	{
		if(enabled)glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
		stencilTestEnabled = enabled;
	}
}

void GraphicsGL::SetFaceCullingEnabled(bool enabled)
{
	if(faceCullingEnabled != enabled || !initialized)
	{
		if(enabled)glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		faceCullingEnabled = enabled;
	}
}

/*
void GraphicsGL::SetRenderBufferEnabled(RenderBufferType buffer, bool enabled) const
{
	if(buffer == RenderBufferType::Color && enabled != colorBufferEnabled)
	{
		if(enabled)glColorMask(1,1,1,1);
		else glColorMask(0,0,0,0);
		colorBufferEnabled = enabled;
	}
	else if(buffer == RenderBufferType::Depth && enabled != depthBufferEnabled)
	{
		if(enabled)glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);
		depthBufferEnabled = enabled;
	}
	else if(buffer == RenderBufferType::Stencil && enabled != stencilBufferEnabled)
	{
		if(enabled)glStencilMask(GL_TRUE);
		else glStencilMask(GL_FALSE);
		stencilBufferEnabled = enabled;
	}
}*/

VertexAttrBuffer * GraphicsGL::CreateVertexAttributeBuffer()
{
	return new VertexAttrBufferGL();
}

void GraphicsGL::DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer)
{
	ASSERT_RTRN(buffer != NULL, "GraphicsGL::DestroyVertexAttributeBuffer -> buffer is NULL");
	delete buffer;
}

Texture * GraphicsGL::CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, TextureAttributes attributes)
{
	glEnable(GL_TEXTURE_2D);
	GLuint tex;
	glGenTextures(1, &tex);
	ASSERT(tex > 0, "GraphicsGL::CreateTexture -> Unable to generate texture", NULL);

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if(attributes.FilterMode == TextureFilter::Point)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if(attributes.FilterMode == TextureFilter::Linear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if(attributes.FilterMode == TextureFilter::BiLinear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if(attributes.FilterMode == TextureFilter::TriLinear)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if(attributes.IsDepthTexture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	}
	else
	{
		if(openGLVersion >= 3)
		{
			if(attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, attributes.MipMapLevel);
			}

			//TODO: Figure out correct way to set texture data for OpenGL >= 3.0
			//glTexStorage2D(GL_TEXTURE_2D, attributes.MipMapLevel, GL_RGBA8, raw->GetWidth(), raw->GetHeight());
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, raw->GetWidth(), raw->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, raw->GetPixels());
			//glTextureSubImage2D(tex, 0, 0, 0, raw->GetWidth(), raw->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, raw->GetPixels());

			if(pixelData != NULL)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

			if(attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear)glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			if(attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, attributes.MipMapLevel);
			}

			if(pixelData != NULL)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
	}

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
	glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );

	glBindTexture(GL_TEXTURE_2D, 0);

	TextureGL * texture = new TextureGL(attributes, tex);
	return texture;
}

Texture * GraphicsGL::CreateTexture(RawImage * imageData,  TextureAttributes attributes)
{
	ASSERT(imageData != NULL, "GraphicsGL::CreateTexture -> imageData is NULL", NULL);
	Texture * texture =  CreateTexture(imageData->GetWidth(), imageData->GetHeight(), imageData->GetPixels(), attributes);
	if(texture != NULL)
	{
		TextureGL * texGL = dynamic_cast<TextureGL*>(texture);
		if(texGL != NULL)
		{
			texGL->AddImageData(imageData);
		}
	}
	return texture;
}

Texture * GraphicsGL::CreateTexture(const std::string& sourcePath, TextureAttributes attributes)
{
	RawImage * raw = ImageLoader::LoadImage(sourcePath);
	ASSERT(raw != NULL, "GraphicsGL::CreateTexture -> unable to create raw image", NULL);

	TextureGL * tex = (TextureGL*)CreateTexture(raw, attributes);
	if(tex == NULL)Debug::PrintError("GraphicsGL::CreateTexture -> Unable to create texture.");

	return tex;
}

Texture * GraphicsGL::CreateCubeTexture(RawImage * frontData,  RawImage * backData,  RawImage * topData,
										RawImage * bottomData,  RawImage * leftData,  RawImage * rightData)
{
	ASSERT(frontData != NULL, "GraphicsGL::CreateCubeTexture -> Front image is NULL.", NULL);
	ASSERT(backData != NULL, "GraphicsGL::CreateCubeTexture -> Back image is NULL.", NULL);
	ASSERT(topData != NULL, "GraphicsGL::CreateCubeTexture -> Top image is NULL.", NULL);
	ASSERT(bottomData != NULL, "GraphicsGL::CreateCubeTexture -> Bottom image is NULL.", NULL);
	ASSERT(leftData != NULL, "GraphicsGL::CreateCubeTexture -> Left image is NULL.", NULL);
	ASSERT(rightData != NULL, "GraphicsGL::CreateCubeTexture -> Right image is NULL.", NULL);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	GLuint tex;
	glGenTextures(1, &tex);
	ASSERT(tex > 0, "GraphicsGL::CreateCubeTexture -> unable to generate texture", NULL);

	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, frontData->GetWidth(), frontData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, frontData->GetPixels());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, backData->GetWidth(), backData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backData->GetPixels());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, topData->GetWidth(), topData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, topData->GetPixels());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, bottomData->GetWidth(), bottomData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bottomData->GetPixels());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, leftData->GetWidth(), leftData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, leftData->GetPixels());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, rightData->GetWidth(), rightData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rightData->GetPixels());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureAttributes attributes;
	attributes.WrapMode = TextureWrap::Clamp;
	attributes.FilterMode = TextureFilter::BiLinear;
	attributes.IsCube = true;
	attributes.MipMapLevel = 0;

	TextureGL * texture = new TextureGL(attributes, tex);
	if(texture != NULL)
	{
		texture->AddImageData(frontData);
		texture->AddImageData(backData);
		texture->AddImageData(topData);
		texture->AddImageData(bottomData);
		texture->AddImageData(leftData);
		texture->AddImageData(rightData);
	}

	return texture;
}

Texture * GraphicsGL::CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
									    const std::string& bottom, const std::string& left, const std::string& right)
{
	RawImage * rawFront = ImageLoader::LoadImage(front);
	RawImage * rawBack = ImageLoader::LoadImage(back);
	RawImage * rawTop = ImageLoader::LoadImage(top);
	RawImage * rawBottom = ImageLoader::LoadImage(bottom);
	RawImage * rawLeft = ImageLoader::LoadImage(left);
	RawImage * rawRight = ImageLoader::LoadImage(right);

	TextureGL * tex = NULL;
	if(rawFront != NULL && rawBack != NULL && rawTop != NULL &&
	   rawBottom != NULL && rawLeft != NULL && rawRight != NULL)
	{
		std::vector<RawImage*> imageData;
		std::vector<std::string> sourcePaths;

		tex = (TextureGL*)CreateCubeTexture(rawFront, rawBack, rawTop,
											rawBottom, rawLeft, rawRight);
		if(tex == NULL)Debug::PrintError("GraphicsGL::CreateCubeTexture -> Unable to create texture.");
	}
	else
	{
		Debug::PrintError("GraphicsGL::CreateCubeTexture -> Unable to load cube map texture.");
	}

	return tex;
}

void GraphicsGL::DestroyTexture(Texture * texture)
{
	ASSERT_RTRN(texture != NULL, "GraphicsGL::DestroyTexture -> texture is NULL");

	TextureGL * texGL = dynamic_cast<TextureGL*>(texture);
	ASSERT_RTRN(texGL != NULL, "GraphicsGL::DestroyTexture -> texture is not OpenGL compatible");

	GLuint textureID = texGL->GetTextureID();
	if(glIsTexture(textureID))
	{
		glDeleteTextures(1,&textureID);
	}
	delete texGL;
}

RenderTarget * GraphicsGL::CreateRenderTarget(bool hasColor, bool hasDepth, unsigned int width, unsigned int height)
{
	RenderTargetGL * buffer;
	buffer = new RenderTargetGL(hasColor, hasDepth, width, height);
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

void GraphicsGL::SetBlendingEnabled(bool enabled)
{
	if(blendingEnabled != enabled || !initialized)
	{
		if(enabled)glEnable(GL_BLEND);
		else glDisable(GL_BLEND);
		blendingEnabled = enabled;
	}
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
		case BlendingProperty::Zero:
			return GL_ZERO;
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

void GraphicsGL::EnterRenderMode(RenderMode renderMode)
{
	unsigned int clearBufferMask = 0;

	switch(renderMode)
	{
		case RenderMode::ShadowVolumeRender:

			// disable rendering to the color buffer
			SetColorBufferChannelState(false,false,false,false);
			SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);
			SetDepthBufferReadOnly(true);
			SetFaceCullingEnabled(false);

			// GL_DEPTH_CLAMP == true means no near or far clipping, achieves same effect
			// as infinite far place projection matrix, which is necessary because the back
			// vertices of the shadow volume will be projected to infinity.
			glEnable(GL_DEPTH_CLAMP);

			clearBufferMask = 0;
			IntMaskUtil::SetBitForMask(&clearBufferMask, (unsigned int)RenderBufferType::Stencil);
			Engine::Instance()->GetGraphicsEngine()->ClearRenderBuffers(clearBufferMask);
			SetStencilBufferEnabled(true);
			SetStencilTestEnabled(true);

			// We need the stencil test to be enabled but we want it
			// to succeed always. Only the depth test matters.
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilMask( 0xff );

			// Set the stencil test per the depth fail algorithm
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			SetBlendingEnabled(false);

		break;
		case RenderMode::StandardWithShadowVolumeTest:

			// enable color buffer rendering
			SetColorBufferChannelState(true,true,true,true);
			SetDepthBufferReadOnly(false);
			SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);

			// enable near & far clipping planes
			glDisable(GL_DEPTH_CLAMP);

			SetStencilTestEnabled(true);

			 // Draw only if the corresponding stencil value is zero
			glStencilFunc(GL_EQUAL, 0x0, 0xFF);

			// prevent update to the stencil buffer
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			SetFaceCullingEnabled(true);

			SetBlendingEnabled(false);

		break;
		case RenderMode::DepthOnly:

			// disable color buffer rendering
			SetColorBufferChannelState(false,false,false,false);
			SetDepthBufferReadOnly(false);
			SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);

			// enable near & far clipping planes
			glDisable(GL_DEPTH_CLAMP);

			SetStencilTestEnabled(false);

			SetFaceCullingEnabled(true);

			SetBlendingEnabled(false);

		break;
		default:
		case RenderMode::Standard:

			// enable color buffer rendering
			SetColorBufferChannelState(true,true,true,true);
			SetDepthBufferReadOnly(false);
			SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);

			// enable near & far clipping planes
			glDisable(GL_DEPTH_CLAMP);

			SetStencilTestEnabled(false);

			SetFaceCullingEnabled(true);

			SetBlendingEnabled(false);

		break;
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
	Graphics::Update();
}

void GraphicsGL::_glutDisplayFunc()
{
	Engine::Instance()->Update();
}

void GraphicsGL::_glutIdleFunc()
{
	//printf("glut idle!\n");
	 glutPostRedisplay();
}

void GraphicsGL::_glutReshapeFunc(int w, int h)
{
	//printf("glut reshape!\n");
	glutPostRedisplay();
}

bool GraphicsGL::ActivateRenderTarget(RenderTargetRef target)
{
	ASSERT(target.IsValid(), "RenderTargetGL::ActiveRenderTarget -> Render target is not valid.", false);
	RenderTargetGL * targetGL = dynamic_cast<RenderTargetGL *>(target.GetPtr());

	ASSERT(targetGL != NULL, "RenderTargetGL::ActiveRenderTarget -> Render target is not a valid OpenGL render target.", false);

	glBindFramebuffer(GL_FRAMEBUFFER, targetGL->GetFBOID());

	return true;
}

bool GraphicsGL::RestoreDefaultRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void GraphicsGL::RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, unsigned int vertexCount, bool validate)
{
	MaterialRef currentMaterial = GetActiveMaterial();

	VertexAttrBufferBinding binding;
	for(unsigned int b = 0; b < boundBuffers.size(); b++)
	{
		binding = boundBuffers[b];
		if(binding.Attribute != StandardAttribute::_None)
		{
			currentMaterial->SendStandardAttributeBufferToShader(binding.Attribute, binding.Buffer);
		}
	}

	// validate the shader variables (attributes and uniforms) that have been set
	if(validate && !currentMaterial->VerifySetVars(vertexCount))return;

	// render the mesh
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

