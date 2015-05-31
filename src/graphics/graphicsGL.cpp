#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "graphics/gl_include.h"
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
#include "object/engineobjectmanager.h"
#include "geometry/transform.h"
#include "global/global.h"
#include "util/time.h"

namespace GTE
{
	/*
	* Single constructor - initialize all member variables.
	*/
	GraphicsGL::GraphicsGL() : Graphics()
	{
		openGLVersion = 0;
		blendingEnabled = false;
		colorBufferEnabled = false;

		depthBufferEnabled = false;
		depthBufferReadOnly = false;

		stencilTestEnabled = false;
		stencilBufferEnabled = false;

		faceCullingMode = FaceCullingMode::Back;
		faceCullingEnabled = true;

		initialized = false;

		redBits = -1;
		greenBits = -1;
		blueBits = -1;
		alphaBits = -1;
		depthBufferBits = -1;
		stencilBufferBits = -1;

		activeClipPlanes = 0;

		openGLMinorVersion = 0;
		openGLVersion = 0;

		window = NULL;
	}

	/*
	 * Clean up.
	 */
	GraphicsGL::~GraphicsGL()
	{

	}

	/*
	 * Initialize the graphics system. The custom properties to be used
	 * during initialization such as screen dimensions are passed in
	 * via [attributes].
	 */
	Bool GraphicsGL::Init(const GraphicsAttributes& attributes)
	{
		this->attributes = attributes;

		// initialize GLFW
		if (!glfwInit())
		{
			Debug::PrintError("Unable to initialize GLFW.");
			return false;
		}

		// set up GLFW antialiasing
		if (GraphicsAttributes::IsMSAA(attributes.AAMethod))
		{
			UInt32 msaaSamples = GraphicsAttributes::GetMSAASamples(attributes.AAMethod);
			if (msaaSamples > 0)glfwWindowHint(GLFW_SAMPLES, msaaSamples);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// create a windowed mode window and its OpenGL context 
		window = glfwCreateWindow(attributes.WindowWidth, attributes.WindowHeight, attributes.WindowTitle.c_str(), NULL, NULL);

		//GLFWvidmode vm;
		//glfwGetDesktopMode(&amp;vm);
		//window = glfwOpenWindow(attributes.WindowWidth, attributes.WindowHeight,vm.RedBits, vm.GreenBits, vm.BlueBits, 0, 0, 0, GLFW_WINDOW)

		if (window == NULL)
		{
			Debug::PrintError("Unable to create GLFW window.");
			glfwTerminate();
			return false;
		}

		// Make the window's context current 
		glfwMakeContextCurrent(window);

		if (attributes.WaitForVSync)glfwSwapInterval(1);
		else glfwSwapInterval(0);

		// initialize GLEW
		glewExperimental = GL_TRUE;
		glewInit();

		// get OpenGL version
		if (glewIsSupported("GL_VERSION_3_2"))
		{
			Debug::PrintMessage("Using OpenGL 3.2");
			openGLVersion = 3;
			openGLMinorVersion = 2;
		}
		else
		{
			Debug::PrintError("Requires OpenGL 3.2 or greater.");
			glfwTerminate();
			return false;
		}

		// call base Init() method
		Bool parentInit = Graphics::Init(this->attributes);
		if (!parentInit)
		{
			Debug::PrintError("Graphics initialization failure.");
			glfwTerminate();
			return false;
		}

		// TODO: think of a better place for these initial calls
		glClearColor(0, 0, 0, 1);
		glFrontFace(GL_CW);
		glEnable(GL_POINT_SPRITE);

		// disable blending by default
		SetBlendingEnabled(false);

		// enable depth buffer testing and make Read/Write
		SetDepthBufferEnabled(true);
		SetDepthBufferReadOnly(false);
		SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);

		// disable stencil buffer by default
		SetStencilBufferEnabled(false);

		// enable face culling
		SetFaceCullingEnabled(true);
		SetFaceCullingMode(FaceCullingMode::Back);

		// get depth information for the default color buffer
		glGetIntegerv(GL_RED_BITS, &redBits);
		glGetIntegerv(GL_GREEN_BITS, &greenBits);
		glGetIntegerv(GL_BLUE_BITS, &blueBits);
		glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
		// printf("color buffer Bits: %d, %d, %d, %d\n", redBits, greenBits, blueBits, alphaBits);

		// get depth information for the default depth buffer
		glGetIntegerv(GL_DEPTH_BITS, &depthBufferBits);
		//printf("depth buffer bits: %d\n", depthBufferBits);

		// get depth information for the default stencil buffer
		glGetIntegerv(GL_STENCIL_BITS, &stencilBufferBits);
		// printf("stencil buffer bits: %d\n", stencilBufferBits);

		defaultRenderTarget = Graphics::SetupDefaultRenderTarget();
		ASSERT(defaultRenderTarget.IsValid(), "GraphicsGL::Init -> Unable to create default render target.");

		ActivateRenderTarget(defaultRenderTarget);

		GLuint vertex_array;
		glGenVertexArrays(1, &vertex_array);
		glBindVertexArray(vertex_array);

		initialized = true;
		return true;
	}

	/*
	 * Start the graphics system.
	 */
	Bool GraphicsGL::Start()
	{
		Graphics::Start();
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			Engine::Instance()->Update();

			/* Poll for and process events */
			glfwPollEvents();
		}

		End();
		return true;
	}


	/*
	 * Shut down the graphics system
	 */
	void GraphicsGL::End()
	{
		Graphics::End();

		glfwTerminate();
	}

	/*
	* Get the GLFWwindow the is used by GLFW.
	*/
	GLFWwindow* GraphicsGL::GetGLFWWindow()
	{
		return window;
	}

	/*
	 * For now, this method does nothing.
	 */
	void GraphicsGL::PreProcessScene()
	{

	}

	/*
	 * Update is called once per frame from the Engine class.
	 */
	void GraphicsGL::Update()
	{
		Graphics::Update();
	}

	/*
	 * RenderScene is called once per frame from the Engine class.
	 */
	void GraphicsGL::RenderScene()
	{
		Graphics::RenderScene();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	}


	/*
	 * Create a new shader from [shaderSource].
	 */
	Shader * GraphicsGL::CreateShader(const ShaderSource& shaderSource)
	{
		Shader * shader = new ShaderGL(shaderSource);
		ASSERT(shader != NULL, "GraphicsGL::CreateShader -> Unable to allocate new shader.");

		// load, compile, and link the shader into a complete OpenGL shader program
		Bool loadSuccess = shader->Load();
		if (!loadSuccess)
		{
			std::string msg = "GraphicsGL::CreateShader -> could not load shader: ";
			msg += std::string(shaderSource.GetName());
			Engine::Instance()->GetErrorManager()->SetAndReportError(ErrorCode::GENERAL_FATAL, msg);
			return NULL;
		}
		return shader;
	}

	/*
	 * Unload and delete [shader].
	 */
	void GraphicsGL::DestroyShader(Shader * shader)
	{
		NONFATAL_ASSERT(shader != NULL, "GraphicsGL::DestroyShader -> 'shader' is null.", true);
		delete shader;
	}

	/*
	 * Clear the buffers specified in [bufferMask] for the currently bound framebuffer.
	 */
	void GraphicsGL::ClearRenderBuffers(IntMask bufferMask)
	{
		GLbitfield glClearMask = 0;
		if (IntMaskUtil::IsBitSetForMask(bufferMask, (UInt32)RenderBufferType::Color))
		{
			glClearMask |= GL_COLOR_BUFFER_BIT;
		}
		if (IntMaskUtil::IsBitSetForMask(bufferMask, (UInt32)RenderBufferType::Depth))
		{
			SetDepthBufferReadOnly(false);
			glClearMask |= GL_DEPTH_BUFFER_BIT;
		}
		if (IntMaskUtil::IsBitSetForMask(bufferMask, (UInt32)RenderBufferType::Stencil))
		{
			glClearMask |= GL_STENCIL_BUFFER_BIT;
		}

		glClear(glClearMask);
	}

	/*
	 * Set the type of face that will be culled during rendering.
	 */
	void GraphicsGL::SetFaceCullingMode(FaceCullingMode mode)
	{
		if (faceCullingMode != mode || !initialized)
		{
			if (mode == FaceCullingMode::Front)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);
			faceCullingMode = mode;
		}
	}

	/*
	 * Get the current face culling mode.
	 */
	FaceCullingMode GraphicsGL::GetFaceCullingMode()
	{
		return faceCullingMode;
	}

	/*
	 * Eanble/disable face culling.
	 */
	void GraphicsGL::SetFaceCullingEnabled(Bool enabled)
	{
		if (faceCullingEnabled != enabled || !initialized)
		{
			if (enabled)glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			faceCullingEnabled = enabled;
		}
	}

	/*
	 * Enable/disable the color channels for color buffer rendering.
	 *
	 * [r] - Enable/disable the red channel.
	 * [g] - Enable/disable the green channel.
	 * [b] - Enable/disable the blue channel.
	 * [a] - Enable/disable the alpha channel.
	 *
	 */
	void GraphicsGL::SetColorBufferChannelState(Bool r, Bool g, Bool b, Bool a)
	{
		GLboolean red = r == true ? GL_TRUE : GL_FALSE;
		GLboolean green = g == true ? GL_TRUE : GL_FALSE;
		GLboolean blue = b == true ? GL_TRUE : GL_FALSE;
		GLboolean alpha = a == true ? GL_TRUE : GL_FALSE;
		glColorMask(red, green, blue, alpha);
	}

	/*
	 * Enable or disable the depth buffer.
	 */
	void GraphicsGL::SetDepthBufferEnabled(Bool enabled)
	{
		if (depthBufferEnabled != enabled || !initialized)
		{
			if (enabled)glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
			depthBufferEnabled = enabled;
		}
	}

	/*
	 * Toggle write enable on the depth buffer.
	 */
	void GraphicsGL::SetDepthBufferReadOnly(Bool readOnly)
	{
		if (depthBufferReadOnly != readOnly || !initialized)
		{
			if (readOnly)glDepthMask(GL_FALSE);
			else glDepthMask(GL_TRUE);
			depthBufferReadOnly = readOnly;
		}
	}

	/*
	 * Set the test that is used when performing depth-buffer occlusion.
	 */
	void GraphicsGL::SetDepthBufferFunction(DepthBufferFunction function)
	{
		switch (function)
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

	/*
	 * Enable/disable the stencil buffer.
	 */
	void GraphicsGL::SetStencilBufferEnabled(Bool enabled)
	{
		if (stencilBufferEnabled != enabled || !initialized)
		{
			if (enabled)glEnable(GL_STENCIL_BUFFER);
			else glDisable(GL_STENCIL_BUFFER);
			stencilBufferEnabled = enabled;
		}
	}

	/*
	 * Enable/disable stencil testing.
	 */
	void GraphicsGL::SetStencilTestEnabled(Bool enabled)
	{
		if (stencilTestEnabled != enabled || !initialized)
		{
			if (enabled)glEnable(GL_STENCIL_TEST);
			else glDisable(GL_STENCIL_TEST);
			stencilTestEnabled = enabled;
		}
	}


	/*
	void GraphicsGL::SetRenderBufferEnabled(RenderBufferType buffer, Bool enabled) const
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

	/*
	 * Create an OpenGL-specific vertex attribute buffer.
	 */
	VertexAttrBuffer * GraphicsGL::CreateVertexAttributeBuffer()
	{
		return new VertexAttrBufferGL();
	}

	/*
	 * Destroy the instance of VertexAttrBuffer pointed to by [buffer].
	 */
	void GraphicsGL::DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer)
	{
		NONFATAL_ASSERT(buffer != NULL, "GraphicsGL::DestroyVertexAttributeBuffer -> 'buffer' is null", true);
		delete buffer;
	}

	/*
	 * Create a 2D OpenGL texture and encapsulate it in a Texture object.
	 *
	 * [width] - Width of the texture in pixels.
	 * [height] - Height of the texture in pixels.
	 * [pixelData] - The source pixels. This parameter can be null, in which case the texture will be blank.
	 * [attributes] - The properties of the texture to be created (format, filtering method, etc...)
	 *
	 */
	Texture * GraphicsGL::CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, const TextureAttributes&  attributes)
	{
		glEnable(GL_TEXTURE_2D);
		GLuint tex;

		// generate the OpenGL texture
		glGenTextures(1, &tex);
		ASSERT(tex > 0, "GraphicsGL::CreateTexture -> Unable to generate texture");

		// make the new texture active
		glBindTexture(GL_TEXTURE_2D, tex);

		// set the wrap mode
		if (attributes.WrapMode == TextureWrap::Mirror)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else if (attributes.WrapMode == TextureWrap::Repeat)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		// set the filter mode. if bi-linear or tri-linear filtering is used,
		// we will be using mip-maps
		if (attributes.FilterMode == TextureFilter::Point)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if (attributes.FilterMode == TextureFilter::Linear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (attributes.FilterMode == TextureFilter::BiLinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (attributes.FilterMode == TextureFilter::TriLinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// depth textures require special set-up
		if (attributes.IsDepthTexture)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
		else
		{
			GLvoid *pixels = pixelData;
			if (pixelData == NULL)pixels = (GLvoid*)0;

			// we only generate mip-maps if bi-linear or tri-linear filtering is used
			if (attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, attributes.MipMapLevel);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, attributes.MipMapLevel);
			}

			// set the texture format, dimensions and data
			glTexImage2D(GL_TEXTURE_2D, 0, GetGLTextureFormat(attributes.Format), width, height, 0, GetGLPixelFormat(attributes.Format), GetGLPixelType(attributes.Format), pixels);
			if (openGLVersion >= 3 && (attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear))glGenerateMipmap(GL_TEXTURE_2D);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		TextureGL * texture = new TextureGL(attributes, tex);
		ASSERT(texture != NULL, "GraphicsGL::CreateTexture -> Unable to allocate TextureGL object.");

		// assign a RawImage object to the texture
		RawImage  * imageData = new RawImage(width, height);
		ASSERT(imageData != NULL, "GraphicsGL::CreateTexture -> Unable to allocate raw image data.");
		ASSERT(imageData->Init(), "GraphicsGL::CreateTexture -> Unable to initialize raw image data.");

		if (pixelData != NULL)imageData->SetDataTo(pixelData);
		texture->AddImageData(imageData);

		return texture;
	}

	/*
	 * Create an OpenGL texture from a RawImage object.
	 *
	 * [imageData] - The RawImage object that contains the pixel data for the texture.
	 * [attributes] - The properties of the texture to be created (format, filtering method, etc...)
	 */
	Texture * GraphicsGL::CreateTexture(RawImage * imageData, const TextureAttributes&  attributes)
	{
		NONFATAL_ASSERT_RTRN(imageData != NULL, "GraphicsGL::CreateTexture -> 'imageData' is null", NULL, true);

		Texture * texture = CreateTexture(imageData->GetWidth(), imageData->GetHeight(), imageData->GetPixels(), attributes);
		return texture;
	}

	/*
	 * Create an OpenGL texture from an image on disk.
	 *
	 * [sourcePath] - Path to the image on disk.
	 * [attributes] - The properties of the texture to be created (format, filtering method, etc...)
	 */
	Texture * GraphicsGL::CreateTexture(const std::string& sourcePath, const TextureAttributes&  attributes)
	{
		RawImage * raw = ImageLoader::LoadImageU(sourcePath);

		if (raw == NULL)
		{
			Engine::Instance()->GetErrorManager()->AddAndReportError(ErrorCode::GENERAL_NONFATAL, "GraphicsGL::CreateTexture -> could not load texture image.");
			return NULL;
		}

		TextureGL * tex = (TextureGL*)CreateTexture(raw, attributes);
		NONFATAL_ASSERT_RTRN(tex != NULL, "GraphicsGL::CreateTexture -> Unable to create texture.", NULL, false);

		return tex;
	}

	/*
	 * Create an OpenGL cube texture for cube mapping, and encapsulate in a Texture object.
	 *
	 * [frontData] - Pixel data for the front of the cube.
	 * [fw], [fh] - Width and height (respectively) of the front image.
	 * [backData] - Pixel data for the back of the cube.
	 * [backw], [backh] - Width and height (respectively) of the back image.
	 * [topData] - Pixel data for the top of the cube.
	 * [tw], [th] - Width and height (respectively) of the top image.
	 * [bottomData] - Pixel data for the bottom of the cube.
	 * [botw], [both] - Width and height (respectively) of the bottom image.
	 * [leftData] - Pixel data for the left side of the cube.
	 * [lw], [lh] - Width and height (respectively) of the left image.
	 * [rightData] - Pixel data for the right side of the cube.
	 * [rw], [rh] - Width and height (respectively) of the right image.
	 */
	Texture * GraphicsGL::CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
		Byte * backData, UInt32 backw, UInt32 backh,
		Byte * topData, UInt32 tw, UInt32 th,
		Byte * bottomData, UInt32 botw, UInt32 both,
		Byte * leftData, UInt32 lw, UInt32 lh,
		Byte * rightData, UInt32 rw, UInt32 rh)
	{
		GLvoid * frontPixels = frontData != NULL ? frontData : (GLvoid*)0;
		GLvoid * backPixels = backData != NULL ? backData : (GLvoid*)0;
		GLvoid * topPixels = topData != NULL ? topData : (GLvoid*)0;
		GLvoid * bottomPixels = bottomData != NULL ? bottomData : (GLvoid*)0;
		GLvoid * leftPixels = leftData != NULL ? leftData : (GLvoid*)0;
		GLvoid * rightPixels = rightData != NULL ? rightData : (GLvoid*)0;

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		GLuint tex;

		// generate the OpenGL cube texture
		glGenTextures(1, &tex);
		ASSERT(tex > 0, "GraphicsGL::CreateCubeTexture -> unable to generate texture");

		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

		// assign the image data to each side of the cube texture
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, fw, fh, 0, GL_RGBA, GL_UNSIGNED_BYTE, frontPixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, backw, backh, 0, GL_RGBA, GL_UNSIGNED_BYTE, backPixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, topPixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, botw, both, 0, GL_RGBA, GL_UNSIGNED_BYTE, bottomPixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, lw, lh, 0, GL_RGBA, GL_UNSIGNED_BYTE, leftPixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, rw, rh, 0, GL_RGBA, GL_UNSIGNED_BYTE, rightPixels);

		// set the relevant texture properties
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		TextureAttributes attributes;
		attributes.WrapMode = TextureWrap::Clamp;
		attributes.FilterMode = TextureFilter::Linear;
		attributes.IsCube = true;
		attributes.MipMapLevel = 0;

		TextureGL * texture = new TextureGL(attributes, tex);
		ASSERT(texture != NULL, "GraphicsGL::CreateCubeTexture -> Unable to allocate TextureGL object.");

		std::vector<RawImage *> imageDatas;
		Byte * datas[] = { frontData, backData, topData, bottomData, leftData, rightData };
		UInt32 widths[] = { fw, backw, tw, botw, lw, rw };
		UInt32 heights[] = { fh, backh, th, both, lh, rh };

		// allocate RawImage object for each side of cube
		for (UInt32 i = 0; i < 6; i++)
		{
			RawImage  * imageData = new RawImage(widths[i], heights[i]);
			ASSERT(imageData != NULL, "GraphicsGL::CreateCubeTexture -> Unable to allocate RawImage object.");
			ASSERT(imageData->Init(), "GraphicsGL::CreateCubeTexture -> Unable to initialize RawImage object.");

			if (datas[i] != NULL)imageData->SetDataTo(datas[i]);
			imageDatas.push_back(imageData);
		}

		// assign RawImage object for each side of cube
		for (UInt32 i = 0; i < 6; i++)
		{
			texture->AddImageData(imageDatas[i]);
		}

		return texture;
	}

	/*
	 * Create an OpenGL cube texture for cube mapping, and encapsulate in a Texture object.
	 *
	 * [frontData] - Image data for the front of the cube.
	 * [backData] - Image data for the back of the cube.
	 * [topData] - Image data for the top of the cube.
	 * [bottomData] - Image data for the bottom of the cube.
	 * [leftData] - Image data for the left side of the cube.
	 * [rightData] - Image data for the right side of the cube.
	 */
	Texture * GraphicsGL::CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
		RawImage * bottomData, RawImage * leftData, RawImage * rightData)
	{
		NONFATAL_ASSERT_RTRN(frontData != NULL, "GraphicsGL::CreateCubeTexture -> Front image is null.", NULL, true);
		NONFATAL_ASSERT_RTRN(backData != NULL, "GraphicsGL::CreateCubeTexture -> Back image is null.", NULL, true);
		NONFATAL_ASSERT_RTRN(topData != NULL, "GraphicsGL::CreateCubeTexture -> Top image is null.", NULL, true);
		NONFATAL_ASSERT_RTRN(bottomData != NULL, "GraphicsGL::CreateCubeTexture -> Bottom image is null.", NULL, true);
		NONFATAL_ASSERT_RTRN(leftData != NULL, "GraphicsGL::CreateCubeTexture -> Left image is null.", NULL, true);
		NONFATAL_ASSERT_RTRN(rightData != NULL, "GraphicsGL::CreateCubeTexture -> Right image is null.", NULL, true);

		Texture * texture = CreateCubeTexture(frontData->GetPixels(), frontData->GetWidth(), frontData->GetHeight(),
			backData->GetPixels(), backData->GetWidth(), backData->GetHeight(),
			topData->GetPixels(), topData->GetWidth(), topData->GetHeight(),
			bottomData->GetPixels(), bottomData->GetWidth(), bottomData->GetHeight(),
			leftData->GetPixels(), leftData->GetWidth(), leftData->GetHeight(),
			rightData->GetPixels(), rightData->GetWidth(), rightData->GetHeight());

		TextureGL * textureGL = dynamic_cast<TextureGL *>(texture);
		ASSERT(textureGL != NULL, "GraphicsGL::CreateCubeTexture -> Unable to cast to TextureGL.");

		return texture;
	}

	/*
	 * Create an OpenGL cube texture for cube mapping, and encapsulate in a Texture object.
	 *
	 * [front] - The path in the file-system for the image file for the front of the cube.
	 * [back] - The path in the file-system for the image file for the back of the cube.
	 * [top] - The path in the file-system for the image file for the top of the cube.
	 * [bottom] -The path in the file-system for the image file for the bottom of the cube.
	 * [left] - The path in the file-system for the image file for the left of the cube.
	 * [right] - The path in the file-system for the image file for the right of the cube.
	 */
	Texture * GraphicsGL::CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
		const std::string& bottom, const std::string& left, const std::string& right)
	{
		RawImage * rawFront = ImageLoader::LoadImageU(front);
		RawImage * rawBack = ImageLoader::LoadImageU(back);
		RawImage * rawTop = ImageLoader::LoadImageU(top);
		RawImage * rawBottom = ImageLoader::LoadImageU(bottom);
		RawImage * rawLeft = ImageLoader::LoadImageU(left);
		RawImage * rawRight = ImageLoader::LoadImageU(right);

		TextureGL * tex = NULL;

		if (rawFront == NULL || rawBack == NULL || rawTop == NULL ||
			rawBottom == NULL || rawLeft == NULL || rawRight == NULL)
		{
			Engine::Instance()->GetErrorManager()->AddAndReportError(ErrorCode::GENERAL_NONFATAL, "GraphicsGL::CreateCubeTexture -> Unable to load cube map texture.");
			return NULL;
		}

		std::vector<RawImage*> imageData;
		std::vector<std::string> sourcePaths;

		tex = (TextureGL*)CreateCubeTexture(rawFront, rawBack, rawTop, rawBottom, rawLeft, rawRight);
		NONFATAL_ASSERT_RTRN(tex != NULL, "GraphicsGL::CreateCubeTexture -> Unable to create texture.", NULL, false);

		return tex;
	}

	/*
	 * Destroy the Texture object specified by [texture].
	 */
	void GraphicsGL::DestroyTexture(Texture * texture)
	{
		NONFATAL_ASSERT(texture != NULL, "GraphicsGL::DestroyTexture -> 'texture' is null", true);

		TextureGL * texGL = dynamic_cast<TextureGL*>(texture);
		ASSERT(texGL != NULL, "GraphicsGL::DestroyTexture -> 'texture' is not OpenGL compatible.");

		GLuint textureID = texGL->GetTextureID();
		if (glIsTexture(textureID))
		{
			glDeleteTextures(1, &textureID);
		}
		delete texGL;
	}

	/*
	 * Create an render target for OpenGL. For OpenGL, the RenderTargetGL() encapsulates the concept
	 * of an off-screen render target.
	 *
	 * [hasColor] - If true, the render target will have a color render texture component.
	 * [hasDepth] - If true, the render target will have a depth render texture component.
	 * [colorTextureAttributes] - Texture attributes that describe the properties of the color render texture.
	 * [width] - Width of both the color and depth render textures.
	 * [height] - Width of both the color and depth render textures.
	 */
	RenderTarget * GraphicsGL::CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
		const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height)
	{
		RenderTargetGL * buffer;
		buffer = new RenderTargetGL(hasColor, hasDepth, enableStencilBuffer, colorTextureAttributes, width, height);
		ASSERT(buffer != NULL, "GraphicsGL::CreateRenderTarget -> Unable to allocate render target.");
		return buffer;
	}

	RenderTarget * GraphicsGL::CreateDefaultRenderTarget()
	{
		TextureAttributes colorAttributes;
		RenderTargetGL * defaultTarget = new RenderTargetGL(false, false, false, colorAttributes, this->attributes.WindowWidth, this->attributes.WindowHeight);
		ASSERT(defaultTarget != NULL, "GraphicsGL::CreateDefaultRenderTarget -> Unable to allocate default render target");

		return defaultTarget;
	}

	/*
	 * Destroy the render target specified by [target].
	 */
	void GraphicsGL::DestroyRenderTarget(RenderTarget * target)
	{
		NONFATAL_ASSERT(target != NULL, "GraphicsGL::DestroyRenderTarget -> 'target' is null", true);

		RenderTargetGL * targetGL = dynamic_cast<RenderTargetGL*>(target);
		if (targetGL != NULL)
		{
			delete targetGL;
		}
	}

	/*
	 * Get the default render target for the graphics system.
	 */
	RenderTargetRef GraphicsGL::GetDefaultRenderTarget()
	{
		return defaultRenderTarget;
	}

	/*
	 * Enable/disable blending.
	 */
	void GraphicsGL::SetBlendingEnabled(Bool enabled)
	{
		if (blendingEnabled != enabled || !initialized)
		{
			if (enabled)glEnable(GL_BLEND);
			else glDisable(GL_BLEND);
			blendingEnabled = enabled;
		}
	}

	/*
	 * Set the type of blending to be used when it is enabled.
	 */
	void GraphicsGL::SetBlendingFunction(BlendingProperty source, BlendingProperty dest)
	{
		glBlendFunc(GetGLBlendProperty(source), GetGLBlendProperty(dest));
	}

	/*
	 * Map BlendingProperty elements to OpenGL blending values.
	 */
	GLenum GraphicsGL::GetGLBlendProperty(BlendingProperty property)
	{
		switch (property)
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

	/*
	 * Activate a material, meaning its shader, attributes, and uniforms will be used for all rendering
	 * calls while it is active.
	 */
	void GraphicsGL::ActivateMaterial(MaterialRef material)
	{
		NONFATAL_ASSERT(material.IsValid(), "GraphicsGL::ActivateMaterial -> 'material' is invalid", true);

		if (!this->activeMaterial.IsValid() || !(this->activeMaterial->GetObjectID() == material->GetObjectID()))
		{
			GLuint oldActiveProgramID = (GLuint)0xFFFFFFF0;
			if (this->activeMaterial.IsValid())
			{
				ShaderRef currentShader = this->activeMaterial->GetShader();
				if (currentShader.IsValid())
				{
					ShaderGL * currentShaderGL = dynamic_cast<ShaderGL *>(currentShader.GetPtr());
					if (currentShaderGL != NULL)
					{
						// get the shader ID
						oldActiveProgramID = currentShaderGL->GetProgramID();
					}
				}
			}

			// call base method
			Graphics::ActivateMaterial(material);

			ShaderRef shader = material->GetShader();
			NONFATAL_ASSERT(shader.IsValid(), "GraphicsGL::ActivateMaterial -> 'shader' is null.", true);

			ShaderGL * shaderGL = dynamic_cast<ShaderGL *>(shader.GetPtr());
			ASSERT(shaderGL != NULL, "GraphicsGL::ActivateMaterial -> Material's shader is not ShaderGL !!");

			// only active the new shader if it is different from the currently active one
			if (oldActiveProgramID != shaderGL->GetProgramID())
			{
				// OpenGL call to activate the shader for [material]
				glUseProgram(shaderGL->GetProgramID());
			}
		}
	}

	/*
	 * A 'render mode' is a grouping of various state values (blending enabled/disabled, face culling
	 * enabled/disabled, stencil test enabled/disabled, etc.). Rather than set each individually,
	 * the concept of 'render mode' was defined so that all the state value for a given type of
	 * rendering could be set at once.
	 */
	void GraphicsGL::EnterRenderMode(RenderMode renderMode)
	{
		UInt32 clearBufferMask = 0;

		switch (renderMode)
		{
		case RenderMode::ShadowVolumeRender:

			// disable rendering to the color buffer
			SetColorBufferChannelState(false, false, false, false);
			SetDepthBufferFunction(DepthBufferFunction::LessThanOrEqual);
			SetDepthBufferReadOnly(true);
			SetFaceCullingEnabled(false);

			// GL_DEPTH_CLAMP == true means no near or far clipping, achieves same effect
			// as infinite far plane projection matrix, which is necessary because the back
			// vertices of the shadow volume will be projected to infinity.
			glEnable(GL_DEPTH_CLAMP);

			clearBufferMask = 0;
			IntMaskUtil::SetBitForMask(&clearBufferMask, (UInt32)RenderBufferType::Stencil);
			Engine::Instance()->GetGraphicsSystem()->ClearRenderBuffers(clearBufferMask);
			SetStencilBufferEnabled(true);
			SetStencilTestEnabled(true);

			// We need the stencil test to be enabled but we want it
			// to succeed always. Only the depth test matters.
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilMask(0xff);

			// Set the stencil test per the depth fail algorithm
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			SetBlendingEnabled(false);

			break;
		case RenderMode::StandardWithShadowVolumeTest:

			// enable color buffer rendering
			SetColorBufferChannelState(true, true, true, true);
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
			SetColorBufferChannelState(false, false, false, false);
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
			SetColorBufferChannelState(true, true, true, true);
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

	/*
	 * Get the version of OpenGL installed on this system.
	 */
	UInt32 GraphicsGL::GetOpenGLVersion()
	{
		return openGLVersion;
	}

	/*
	 * Get the OpenGL constant for texture cube side that corresponds
	 * to [side].
	 */
	GLenum GraphicsGL::GetGLCubeTarget(CubeTextureSide side)
	{
		GLenum target;
		switch (side)
		{
		case CubeTextureSide::Back:
			target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
		case CubeTextureSide::Front:
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
		case CubeTextureSide::Top:
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
		case CubeTextureSide::Bottom:
			target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
		case CubeTextureSide::Left:
			target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
		case CubeTextureSide::Right:
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
		}

		return target;
	}

	/*
	 * Get the OpenGL texture format that corresponds to [format].
	 */
	GLenum GraphicsGL::GetGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R32F:
			return GL_R32F;
			break;
		case TextureFormat::RGBA8:
			return GL_RGBA8;
			break;
		case TextureFormat::RGBA16F:
			return GL_RGBA16F;
			break;
		case TextureFormat::RGBA32F:
			return GL_RGBA32F;
			break;

		}

		return GL_RGBA8;
	}


	/*
	 * Get the OpenGL pixel format that corresponds to [format].
	 */
	GLenum GraphicsGL::GetGLPixelFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R32F:
			return GL_RED;
			break;
		case TextureFormat::RGBA8:
			return GL_RGBA;
			break;
		case TextureFormat::RGBA16F:
			return GL_RGBA;
			break;
		case TextureFormat::RGBA32F:
			return GL_RGBA;
			break;

		}

		return GL_RGBA;
	}


	/*
	 * Get the OpenGL pixel type that corresponds to [format].
	 */
	GLenum GraphicsGL::GetGLPixelType(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R32F:
			return GL_FLOAT;
			break;
		case TextureFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
			break;
		case TextureFormat::RGBA16F:
			return GL_FLOAT;
			break;
		case TextureFormat::RGBA32F:
			return GL_FLOAT;
			break;

		}

		return GL_UNSIGNED_BYTE;
	}

	/*
	 * Make [target] the target for all standard rendering operations.
	 */
	Bool GraphicsGL::ActivateRenderTarget(RenderTargetRef target)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "RenderTargetGL::ActiveRenderTarget -> 'target' is not valid.", false, true);

		RenderTargetGL * targetGL = dynamic_cast<RenderTargetGL *>(target.GetPtr());
		ASSERT(targetGL != NULL, "RenderTargetGL::ActiveRenderTarget -> Render target is not a valid OpenGL render target.");

		if (currentRenderTarget.IsValid())
		{
			// prevent activating the currently active target.
			RenderTargetGL * currentTargetGL = dynamic_cast<RenderTargetGL *>(currentRenderTarget.GetPtr());
			if (currentTargetGL->GetFBOID() == targetGL->GetFBOID())return true;
		}

		glViewport(0, 0, target->GetWidth(), target->GetHeight());

		glBindFramebuffer(GL_FRAMEBUFFER, targetGL->GetFBOID());

		currentRenderTarget = target;

		return true;
	}

	/*
	 * Get the currently active render target.
	 */
	RenderTargetRef GraphicsGL::GetCurrrentRenderTarget()
	{
		return currentRenderTarget;
	}

	/*
	 * For the current render target, if it is cubed, activate [side] as the target for rendering.
	 */
	Bool GraphicsGL::ActivateCubeRenderTargetSide(CubeTextureSide side)
	{
		if (currentRenderTarget.IsValid())
		{
			RenderTargetGL * currentTargetGL = dynamic_cast<RenderTargetGL *>(currentRenderTarget.GetPtr());
			ASSERT(currentTargetGL != NULL, "GraphicsGL::ActivateCubeRenderTargetSide -> Render target is not a valid OpenGL render target.");

			if (!currentTargetGL->GetColorTexture()->GetAttributes().IsCube)
			{
				std::string msg = "GraphicsGL::ActivateCubeRenderTargetSide -> Render target is not cubed.";
				Engine::Instance()->GetErrorManager()->SetAndReportError(GraphicsError::InvalidRenderTarget, msg);
				return false;
			}

			TextureGL * texGL = dynamic_cast<TextureGL*>(currentTargetGL->GetColorTexture().GetPtr());
			ASSERT(texGL != NULL, "GraphicsGL::ActivateCubeRenderTargetSide -> Render target texture is not a valid OpenGL texture.");

			GLenum target = GetGLCubeTarget(side);

			UInt32 sideIndex = (UInt32)side;
			RawImage * imageData = texGL->GetImageData(sideIndex);
			ASSERT(imageData, "GraphicsGL::ActivateCubeRenderTargetSide -> Unable to get image data for specified side.");

			glViewport(0, 0, imageData->GetWidth(), imageData->GetHeight());

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texGL->GetTextureID(), 0);
		}

		return false;
	}

	/*
	 * Make the default framebuffer the active render target.
	 */
	Bool GraphicsGL::RestoreDefaultRenderTarget()
	{
		ActivateRenderTarget(defaultRenderTarget);
		return true;
	}

	/*
	 * Copy the contents of one render target to another.
	 */
	void GraphicsGL::CopyBetweenRenderTargets(RenderTargetRef src, RenderTargetRef dest)
	{
		GLuint currentFB = 0;
		if (currentRenderTarget.IsValid())
		{
			RenderTargetGL * currentGL = dynamic_cast<RenderTargetGL*>(currentRenderTarget.GetPtr());
			if (currentGL != NULL)
			{
				currentFB = currentGL->GetFBOID();
			}
		}

		NONFATAL_ASSERT(src.IsValid(), "GraphicsGL::CopyBetweenRenderTargets -> Source is not valid", true);
		NONFATAL_ASSERT(dest.IsValid(), "GraphicsGL::CopyBetweenRenderTargets -> Destination is not valid", true);

		RenderTargetGL * srcGL = dynamic_cast<RenderTargetGL*>(src.GetPtr());
		ASSERT(srcGL != NULL, "GraphicsGL::CopyBetweenRenderTargets -> Source is not a valid OpenGL render target.");

		RenderTargetGL * destGL = dynamic_cast<RenderTargetGL*>(dest.GetPtr());
		ASSERT(destGL != NULL, "GraphicsGL::CopyBetweenRenderTargets -> Destination is not a valid OpenGL render target.");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, srcGL->GetFBOID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destGL->GetFBOID());

		glBlitFramebuffer(0, 0, src->GetWidth(), src->GetHeight(),
			0, 0, dest->GetWidth(), dest->GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFB);
	}

	/*
	 * Set the contents of [texture] be that of [data].
	 */
	void GraphicsGL::SetTextureData(TextureRef texture, Byte * data)
	{
		SetTextureData(texture, data, CubeTextureSide::Front);
	}

	/*
	 * Set the contents of [texture] be that of [data]. If it is a cube texture, update
	 * the side specified by [side].
	 */
	void GraphicsGL::SetTextureData(TextureRef texture, Byte * data, CubeTextureSide side)
	{
		NONFATAL_ASSERT(texture.IsValid(), "GraphicsGL::SetTextureData -> 'texture' is not valid.", true);

		TextureGL * texGL = dynamic_cast<TextureGL*>(texture.GetPtr());
		ASSERT(texGL != NULL, "GraphicsGL::SetTextureData -> Texture is not a valid OpenGL texture.");

		const TextureAttributes attributes = texture->GetAttributes();
		if (attributes.IsCube)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, texGL->GetTextureID());
			RawImage * imageData = texture->GetImageData((UInt32)side);
			glTexImage2D(GetGLCubeTarget(side), 0, GetGLTextureFormat(attributes.Format), imageData->GetWidth(), imageData->GetHeight(), 0,
				GetGLPixelFormat(attributes.Format), GetGLPixelType(attributes.Format), data);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texGL->GetTextureID());
			RawImage * imageData = texture->GetImageData(0);
			glTexImage2D(GL_TEXTURE_2D, 0, GetGLTextureFormat(attributes.Format), imageData->GetWidth(), imageData->GetHeight(), 0,
				GetGLPixelFormat(attributes.Format), GetGLPixelType(attributes.Format), data);
			if (openGLVersion >= 3 && (attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear))glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	/*
	 * Force a rebuild of the mip-maps for [texture].
	 */
	void GraphicsGL::RebuildMipMaps(TextureRef texture)
	{
		TextureGL * texGL = dynamic_cast<TextureGL*>(texture.GetPtr());
		ASSERT(texGL != NULL, "GraphicsGL::RebuildMipMaps -> Texture is not a valid OpenGL texture.");

		const TextureAttributes attributes = texture->GetAttributes();
		if (openGLVersion >= 3 && (attributes.FilterMode == TextureFilter::TriLinear || attributes.FilterMode == TextureFilter::BiLinear))
		{
			if (!attributes.IsCube)
			{
				glBindTexture(GL_TEXTURE_2D, texGL->GetTextureID());
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

	/*
	 * Enable one more clip plane than is currently enabled.
	 */
	Bool GraphicsGL::AddClipPlane()
	{
		NONFATAL_ASSERT_RTRN(activeClipPlanes < Constants::MaxClipPlanes, "GraphicsGL::ActivateClipPlane -> Maximum clip planes exceeded.", false, true);
		glEnable(GL_CLIP_PLANE0 + activeClipPlanes);
		activeClipPlanes++;
		return true;
	}

	/*
	 * Disable all OpenGL clip planes.
	 */
	void GraphicsGL::DeactiveAllClipPlanes()
	{
		for (UInt32 i = 0; i < activeClipPlanes; i++)
		{
			glDisable(GL_CLIP_PLANE0 + i);
		}
		activeClipPlanes = 0;
	}

	/*
	 * Generic function for rendering the attributes for groups of vertices.
	 *
	 * [boundBuffers] - Array of VertexAttrBufferBinding instances, which hold arrays of attributes (normals, positions, UV coordinates, etc.)
	 * 				    in a format suitable for sending to the GPU.
	 * [vertexCount] - Number of vertices being sent to the GPU.
	 * [validate] - Specifies whether or not to validate the shader variables that have been set prior to rendering.
	 */
	void GraphicsGL::RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, UInt32 vertexCount, Bool validate)
	{
		MaterialRef currentMaterial = GetActiveMaterial();

		VertexAttrBufferBinding binding;
		for (UInt32 b = 0; b < boundBuffers.size(); b++)
		{
			binding = boundBuffers[b];
			if (binding.Attribute != StandardAttribute::_None)
			{
				currentMaterial->SendStandardAttributeBufferToShader(binding.Attribute, binding.Buffer);
			}
		}

		// validate the shader variables (attributes and uniforms) that have been set
		if (validate && !currentMaterial->VerifySetVars(vertexCount))return;

		// render the mesh
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
}

