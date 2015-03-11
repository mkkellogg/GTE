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

    public :

    Shader * CreateShader(const ShaderSource& shaderSource);
   void DestroyShader(Shader * shader);
   VertexAttrBuffer * CreateVertexAttributeBuffer();
   void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
   Texture * CreateTexture(const std::string& sourcePath, const TextureAttributes&  attributes);
   Texture * CreateTexture(RawImage * imageData, const TextureAttributes&  attributes);
   Texture * CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, const TextureAttributes&  attributes);
   Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
							const std::string& bottom, const std::string& left, const std::string& right);
   Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
							RawImage * bottomData, RawImage * leftData, RawImage * rightData);
   void DestroyTexture(Texture * texture);
   RenderTarget * CreateRenderTarget(bool hasColor, bool hasDepth, const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height);
   void DestroyRenderTarget(RenderTarget * target);

    RenderTargetRef GetDefaultRenderTarget();
    void ClearRenderBuffers(IntMask bufferMask);

    void SetColorBufferChannelState(bool r, bool g, bool b, bool a);
    void SetDepthBufferEnabled(bool enabled);
    void SetDepthBufferReadOnly(bool readOnly);
    void SetDepthBufferFunction(DepthBufferFunction function);
    void SetStencilBufferEnabled(bool enabled);
    void SetStencilTestEnabled(bool enabled);

    void SetFaceCullingEnabled(bool enabled);

    void SetBlendingEnabled(bool enabled);
    void SetBlendingFunction(BlendingProperty source, BlendingProperty dest);
    GLenum GetGLBlendProperty(BlendingProperty property);
    void ActivateMaterial(MaterialRef material);

    void EnterRenderMode(RenderMode renderMode);

    bool Init(const GraphicsAttributes& attributes);
    unsigned int GetOpenGLVersion();

    bool ActivateRenderTarget(RenderTargetRef target);
    bool RestoreDefaultRenderTarget();

    void RenderTriangles(const std::vector<VertexAttrBufferBinding>& boundBuffers, unsigned int vertexCount, bool validate);
};

#endif
