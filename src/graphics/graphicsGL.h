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
	class RenderTargetGL;
	class RawImage;

	class GraphicsGL : public Graphics
	{
		friend class Graphics;
		friend class Engine;
		friend class SubMesh3DRenderer;

		GLFWwindow* window;

	protected:

		// the material that is currently being used for rendering
		MaterialSharedPtr activeMaterial;
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
		// which faces will be culled during rendering?
		RenderState::FaceCulling faceCullingMode;
		// number of currently active clip planes
		UInt32 activeClipPlanes;
		// RenderTarget objects that encapsulates the OpenGL default framebuffer
		RenderTargetSharedPtr defaultRenderTarget;
		// currently bound render target;
		RenderTargetSharedPtr currentRenderTarget;
		// number of bits per channel in the OpenGL color buffer
		Int32 redBits, greenBits, blueBits, alphaBits;
		// bit depth of the OpenGL depth buffer
		Int32 depthBufferBits;
		// bit depth of the OpenGL stencil buffer
		Int32 stencilBufferBits;

		// is the graphics system initialized?
		Bool initialized;

		GraphicsGL();
		~GraphicsGL();

		// local version of OpenGL
		UInt32 openGLVersion;
		UInt32 openGLMinorVersion;

		Bool Start() override;
		void End() override;
		void Update() override;
		void PostRender() override;

		RenderTarget * CreateDefaultRenderTarget() override;
		Bool ActivateRenderTarget(RenderTargetRef target) override;
		const RenderTargetRef GetCurrrentRenderTarget() override;
		Bool ActivateCubeRenderTargetSide(CubeTextureSide side) override;
		Bool RestoreDefaultRenderTarget() override;

		GLenum GetGLCubeTarget(CubeTextureSide side) const;
		GLenum GetGLTextureFormat(TextureFormat format) const;
		GLenum GetGLPixelFormat(TextureFormat format) const;
		GLenum GetGLPixelType(TextureFormat format) const;

		void GetCurrentBufferBits();

		Shader * CreateShader(const ShaderSource& shaderSource) override;
		void DestroyShader(Shader * shader) override;
		VertexAttrBuffer * CreateVertexAttributeBuffer() override;
		void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer) override;
		Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes&  attributes) override;
		Texture * CreateTexture(RawImage * imageData, const TextureAttributes&  attributes) override;
		Texture * CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, const TextureAttributes&  attributes) override;
		Texture * CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
									Byte * backData, UInt32 backw, UInt32 backh,
									Byte * topData, UInt32 tw, UInt32 th,
									Byte * bottomData, UInt32 botw, UInt32 both,
									Byte * leftData, UInt32 lw, UInt32 lh,
									Byte * rightData, UInt32 rw, UInt32 rh) override;
		Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
									RawImage * bottomData, RawImage * leftData, RawImage * rightData) override;
		Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
									const std::string& bottom, const std::string& left, const std::string& right) override;
		void DestroyTexture(Texture * texture) override;
		RenderTarget * CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
										  const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height) override;
		void DestroyRenderTarget(RenderTarget * target) override;

		RenderTargetSharedPtr GetDefaultRenderTarget() override;

		void SetupStateForMaterial(MaterialRef material, Bool reverseFaceCulling);
		void ActivateMaterial(MaterialRef material, Bool reverseFaceCulling) override;
		MaterialRef GetActiveMaterial() override;

		void EnterRenderMode(RenderMode renderMode) override;		

		void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundAttributeBuffers, UInt32 vertexCount, Bool validate) override;

	public:

		GLFWwindow* GetGLFWWindow();

		void ClearRenderBuffers(IntMask bufferMask) const override;

		void SetFaceCullingMode(RenderState::FaceCulling mode) override;
		RenderState::FaceCulling GetFaceCullingMode() const override;
		void SetFaceCullingEnabled(Bool enabled) override;

		void SetColorBufferChannelState(Bool r, Bool g, Bool b, Bool a) override;
		void SetDepthBufferEnabled(Bool enabled) override;
		void SetDepthBufferReadOnly(Bool readOnly) override;
		void SetDepthBufferFunction(RenderState::DepthBufferFunction function) override;
		void SetStencilBufferEnabled(Bool enabled) override;
		void SetStencilTestEnabled(Bool enabled) override;

		void SetBlendingEnabled(Bool enabled) override;
		void SetBlendingFunction(RenderState::BlendingMethod source, RenderState::BlendingMethod dest) override;
		GLenum GetGLBlendProperty(RenderState::BlendingMethod property) const;	

		Bool Init(const GraphicsAttributes& attributes) override;
		UInt32 GetOpenGLVersion() const;
		
		Bool CanBlitColorBuffers(const RenderTargetGL * src, const RenderTargetGL * dest) const;
		Bool CanBlitDepthBuffers(const RenderTargetGL * src, const RenderTargetGL * dest) const;
		void CopyBetweenRenderTargets(RenderTargetSharedPtr src, RenderTargetSharedConstPtr dest) const override;

		void SetTextureData(TextureSharedPtr texture, const Byte * data) const override;
		void SetTextureData(TextureSharedPtr texture, const Byte * data, CubeTextureSide side) const override;
		void RebuildMipMaps(TextureSharedPtr texture) const override;

		Bool AddClipPlane() override;
		void DeactiveAllClipPlanes() override;		
	};
}

#endif
