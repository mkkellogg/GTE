/*
 * Class: GraphicsGL
 *
 * Author: Mark Kellogg
 *
 * This is the OpenGL graphics system, it is an implementation of the Graphics class.
 *
 * Most of the OpenGL-related functionality in the engine is
 * contained within this class. Through its interface which is not OpenGL
 * specific, all the core OpenGL state variables can be set, and OpenGL
 * objects such as shaders, textures, and framebuffers can be created, activated,
 * or destroyed.

 * Additionally all of the GLFW, GLEW, and OpenGL initialization code is in
 * this class, in the Init() method.
 */


#ifndef _GTE_GRAPHICS_GL_H_
#define _GTE_GRAPHICS_GL_H_

#include "graphics/gl_include.h"
#include "graphics.h"
#include "base/intmask.h"
#include <string>

#define GL_GLEXT_PROTOTYPES

namespace GTE
{
	//forward declarations
	class Shader;
	class SubMesh3DRenderer;
	class SubMesh3DRendererGL;
	class Material;
	class Camera;
	class VertexAttrBuffer;
	class TextureAttributes;
	class AttributeTransformer;
	class RenderTarget;
	class RawImage;

	class GraphicsGL : public Graphics
	{
		friend class Graphics;
		friend class Engine;

		GLFWwindow* window;

	protected:

		// is OpenGL blending enabled?
		Bool blendingEnabled;
		// is OpenGL depth buffer enabled?
		Bool depthBufferEnabled;
		// is OpenGL depth buffer currently read only?
		Bool depthBufferReadOnly;
		// is the OpenGL color buffer enabled?
		Bool colorBufferEnabled;
		// is the OpenGL stencil buffer enabled?
		Bool stencilBufferEnabled;
		// is OpenGL stencil testing enabled?
		Bool stencilTestEnabled;
		// is face culling enabled in OpenGL?
		Bool faceCullingEnabled;
		// number of bits per channel in the OpenGL color buffer
		Int32 redBits, greenBits, blueBits, alphaBits;
		// bit depth of the OpenGL depth buffer
		Int32 depthBufferBits;
		// bit depth of the OpenGL stencil buffer
		Int32 stencilBufferBits;
		// which faces will be culled during rendering?
		FaceCullingMode faceCullingMode;

		// number of currently active clip planes
		UInt32 activeClipPlanes;

		// is the graphics system initialized?
		Bool initialized;

		// RenderTarget objects that encapsulates the OpenGL default framebuffer
		RenderTargetRef defaultRenderTarget;
		// currently bound render target;
		RenderTargetRef currentRenderTarget;

		GraphicsGL();
		~GraphicsGL();

		// local version of OpenGL
		UInt32 openGLVersion;
		UInt32 openGLMinorVersion;

		void PreProcessScene();
		Bool Start();
		void End();
		void Update();
		void RenderScene();

		RenderTarget * CreateDefaultRenderTarget();

		GLenum GetGLCubeTarget(CubeTextureSide side);
		GLenum GetGLTextureFormat(TextureFormat format);
		GLenum GetGLPixelFormat(TextureFormat format);
		GLenum GetGLPixelType(TextureFormat format);

	public:

		GLFWwindow* GetGLFWWindow();

		Shader * CreateShader(const ShaderSource& shaderSource);
		void DestroyShader(Shader * shader);
		VertexAttrBuffer * CreateVertexAttributeBuffer();
		void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
		Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes&  attributes);
		Texture * CreateTexture(RawImage * imageData, const TextureAttributes&  attributes);
		Texture * CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, const TextureAttributes&  attributes);
		Texture * CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
									Byte * backData, UInt32 backw, UInt32 backh,
									Byte * topData, UInt32 tw, UInt32 th,
									Byte * bottomData, UInt32 botw, UInt32 both,
									Byte * leftData, UInt32 lw, UInt32 lh,
									Byte * rightData, UInt32 rw, UInt32 rh);
		Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
									RawImage * bottomData, RawImage * leftData, RawImage * rightData);
		Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
									const std::string& bottom, const std::string& left, const std::string& right);
		void DestroyTexture(Texture * texture);
		RenderTarget * CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
										  const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height);
		void DestroyRenderTarget(RenderTarget * target);

		RenderTargetRef GetDefaultRenderTarget();
		void ClearRenderBuffers(IntMask bufferMask);

		void SetFaceCullingMode(FaceCullingMode mode);
		FaceCullingMode GetFaceCullingMode();
		void SetFaceCullingEnabled(Bool enabled);

		void SetColorBufferChannelState(Bool r, Bool g, Bool b, Bool a);
		void SetDepthBufferEnabled(Bool enabled);
		void SetDepthBufferReadOnly(Bool readOnly);
		void SetDepthBufferFunction(DepthBufferFunction function);
		void SetStencilBufferEnabled(Bool enabled);
		void SetStencilTestEnabled(Bool enabled);

		void SetBlendingEnabled(Bool enabled);
		void SetBlendingFunction(BlendingProperty source, BlendingProperty dest);
		GLenum GetGLBlendProperty(BlendingProperty property);
		void ActivateMaterial(MaterialRef material);

		void EnterRenderMode(RenderMode renderMode);

		Bool Init(const GraphicsAttributes& attributes);
		UInt32 GetOpenGLVersion();

		Bool ActivateRenderTarget(RenderTargetRef target);
		RenderTargetRef GetCurrrentRenderTarget();
		Bool ActivateCubeRenderTargetSide(CubeTextureSide side);
		Bool RestoreDefaultRenderTarget();
		void CopyBetweenRenderTargets(RenderTargetRef src, RenderTargetRef dest);

		void SetTextureData(TextureRef texture, Byte * data);
		void SetTextureData(TextureRef texture, Byte * data, CubeTextureSide side);
		void RebuildMipMaps(TextureRef texture);

		Bool AddClipPlane();
		void DeactiveAllClipPlanes();

		void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, UInt32 vertexCount, Bool validate);
	};
}

#endif
