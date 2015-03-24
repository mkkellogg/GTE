/*
 * Class: GraphicsGL
 *
 * Author: Mark Kellogg
 *
 * This is the OpenGL implementation of the Graphics class.
 *
 * Most of the OpenGL-related functionality in the engine is
 * contained within this class. Through its interface which is not OpenGL
 * specific, all the core OpenGL state variables can be set, and OpenGL
 * objects such as shaders, textures, and framebuffers can be created, activated,
 * or destroyed.

 * Additionally all of the GLUT, GLEW, and OpenGL initialization code is in
 * this class, in the Init() method.
 */


#ifndef _GTE_GRAPHICS_GL_H_
#define _GTE_GRAPHICS_GL_H_

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

#include <GL/glew.h>
#include <GL/glut.h>
#include "graphics.h"
#include "base/intmask.h"
#include <string>

class GraphicsGL : public Graphics
{
    friend class Graphics;
    friend class Engine;

    // static GLUT callbacks
    static void _glutDisplayFunc();
    static void _glutIdleFunc();
    static void _glutReshapeFunc(int w, int h);

    protected:

    // is OpenGL blending enabled?
    bool blendingEnabled;
    // is OpenGL depth buffer enabled?
    bool depthBufferEnabled;
    // is OpenGL depth buffer currently read only?
    bool depthBufferReadOnly;
    // is the OpenGL color buffer enabled?
    bool colorBufferEnabled;
    // is the OpenGL stencil buffer enabled?
    bool stencilBufferEnabled;
    // is OpenGL stencil testing enabled?
    bool stencilTestEnabled;
    // is face culling enabled in OpenGL?
    bool faceCullingEnabled;
    // number of bits per channel in the OpenGL color buffer
    int redBits, greenBits, blueBits, alphaBits;
    // bit depth of the OpenGL depth buffer
    int depthBufferBits;
    // bit depth of the OpenGL stencil buffer
    int stencilBufferBits;
    // which faces will be culled during rendering?
    FaceCullingMode faceCullingMode;

    // number of currently active clip planes
    unsigned int activeClipPlanes;

    // is the graphics engine initialized?
    bool initialized;

    // RenderTarget objects that encapsulates the OpenGL default framebuffer
    RenderTargetRef defaultRenderTarget;
    // currently bound render target;
    RenderTargetRef currentRenderTarget;

    GraphicsGL();
    ~GraphicsGL();

    // local version of OpenGL
    unsigned int openGLVersion;

    void PreProcessScene();
    bool Start();
    void Update();
    void RenderScene();

    RenderTarget * CreateDefaultRenderTarget();

    GLenum GetGLCubeTarget(CubeTextureSide side);
    GLenum GetGLTextureFormat(TextureFormat format);
    GLenum GetGLPixelFormat(TextureFormat format);
    GLenum GetGLPixelType(TextureFormat format);

    public :

    Shader * CreateShader(const ShaderSource& shaderSource);
   void DestroyShader(Shader * shader);
   VertexAttrBuffer * CreateVertexAttributeBuffer();
   void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
   Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes&  attributes);
   Texture * CreateTexture(RawImage * imageData, const TextureAttributes&  attributes);
   Texture * CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, const TextureAttributes&  attributes);
   Texture * CreateCubeTexture(BYTE * frontData, unsigned int fw, unsigned int fh,
		   	   	   	   	   	   BYTE * backData, unsigned int backw, unsigned int backh,
		   	   	   	   	   	   BYTE * topData, unsigned int tw, unsigned int th,
		   	   	   	   	   	   BYTE * bottomData, unsigned int botw, unsigned int both,
		   	   	   	   	   	   BYTE * leftData, unsigned int lw, unsigned int lh,
		   	   	   	   	   	   BYTE * rightData, unsigned int rw, unsigned int rh);
   Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
							RawImage * bottomData, RawImage * leftData, RawImage * rightData);
   Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
							const std::string& bottom, const std::string& left, const std::string& right);
   void DestroyTexture(Texture * texture);
   RenderTarget * CreateRenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer,
		   	   	   	   	   	   	     const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height);
   void DestroyRenderTarget(RenderTarget * target);

    RenderTargetRef GetDefaultRenderTarget();
    void ClearRenderBuffers(IntMask bufferMask);

    void SetFaceCullingMode(FaceCullingMode mode);
    FaceCullingMode GetFaceCullingMode();
    void SetFaceCullingEnabled(bool enabled);

    void SetColorBufferChannelState(bool r, bool g, bool b, bool a);
    void SetDepthBufferEnabled(bool enabled);
    void SetDepthBufferReadOnly(bool readOnly);
    void SetDepthBufferFunction(DepthBufferFunction function);
    void SetStencilBufferEnabled(bool enabled);
    void SetStencilTestEnabled(bool enabled);

    void SetBlendingEnabled(bool enabled);
    void SetBlendingFunction(BlendingProperty source, BlendingProperty dest);
    GLenum GetGLBlendProperty(BlendingProperty property);
    void ActivateMaterial(MaterialRef material);

    void EnterRenderMode(RenderMode renderMode);

    bool Init(const GraphicsAttributes& attributes);
    unsigned int GetOpenGLVersion();

    bool ActivateRenderTarget(RenderTargetRef target);
    RenderTargetRef GetCurrrentRenderTarget();
    bool ActivateCubeRenderTargetSide(CubeTextureSide side);
    bool RestoreDefaultRenderTarget();
    void CopyBetweenRenderTargets(RenderTargetRef src, RenderTargetRef dest);

    void SetTextureData(TextureRef texture, BYTE * data);
    void SetTextureData(TextureRef texture, BYTE * data, CubeTextureSide side);
    void RebuildMipMaps(TextureRef texture);

    bool AddClipPlane();
    void DeactiveAllClipPlanes();

    void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, unsigned int vertexCount, bool validate);
};

#endif
