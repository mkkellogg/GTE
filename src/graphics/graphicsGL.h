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

    Shader * CreateShader(const ShaderSource& shaderSource);
    void DestroyShader(Shader * shader);
    SubMesh3DRenderer * CreateMeshRenderer(AttributeTransformer * attrTransformer);
    void DestroyMeshRenderer(SubMesh3DRenderer * buffer);
    VertexAttrBuffer * CreateVertexAttributeBuffer();
    void DestroyVertexAttributeBuffer(VertexAttrBuffer * buffer);
    Texture * CreateTexture(const std::string& sourcePath,  TextureAttributes attributes);
    Texture * CreateTexture(const RawImage * imageData, const std::string& sourcePath, TextureAttributes attributes);
    void DestroyTexture(Texture * texture);
    RenderTarget * CreateRenderTarget(IntMask buffers, unsigned int width, unsigned int height);
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
    MaterialRef GetActiveMaterial();

    void EnterRenderMode(RenderMode renderMode);

    void PreProcessScene();
    void RenderScene();
    bool Run();
    void Update();

    public :

    bool Init(const GraphicsAttributes& attributes);
    unsigned int GetOpenGLVersion();
};

#endif
