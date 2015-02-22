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
    friend class SubMesh3DRendererGL;

    static void _glutDisplayFunc();
    static void _glutIdleFunc();
    static void _glutReshapeFunc(int w, int h);

    protected:

    bool blendingEnabled;

    bool depthBufferEnabled;
    bool depthBufferReadOnly;

    bool colorBufferEnabled;

    bool stencilBufferEnabled;
    bool stencilTestEnabled;

    bool faceCullingEnabled;

    int redBits, greenBits, blueBits, alphaBits;
    int depthBufferBits;
    int stencilBufferBits;

    bool initialized;

    GraphicsGL();
    ~GraphicsGL();

    unsigned int openGLVersion;

    void PreProcessScene();
    void RenderScene();
    bool Run();
    void Update();


    public :

    Shader * CreateShader(const ShaderSource& shaderSource);
    void DestroyShader(Shader * shader);
    VertexAttrBuffer * CreateVertexAttributeBuffer();
    void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
    Texture * CreateTexture(const std::string& sourcePath, TextureAttributes attributes);
    Texture * CreateTexture(RawImage * imageData, TextureAttributes attributes);
    Texture * CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, TextureAttributes attributes);
    Texture * CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
		    				    const std::string& bottom, const std::string& left, const std::string& right);
    Texture * CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
								RawImage * bottomData, RawImage * leftData, RawImage * rightData);
    void DestroyTexture(Texture * texture);
    RenderTarget * CreateRenderTarget(bool hasColor, bool hasDepth, unsigned int width, unsigned int height);
    void DestroyRenderTarget(RenderTarget * target);

    void ClearRenderBuffers(unsigned int bufferMask);

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
