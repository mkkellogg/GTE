#ifndef _GRAPHICS_GL_H_
#define _GRAPHICS_GL_H_

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

    protected:

    bool blendingEnabled;

    GraphicsGL();
    ~GraphicsGL();

    unsigned int openGLVersion;

    Shader * CreateShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
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

    void ClearBuffers(unsigned int bufferMask) const;

    void EnableBlending(bool enabled);
    void SetBlendingFunction(BlendingProperty source, BlendingProperty dest);
    GLenum GetGLBlendProperty(BlendingProperty property);
    void ActivateMaterial(MaterialRef material);
    MaterialRef GetActiveMaterial();

    void PreProcessScene();
    void RenderScene();
    bool Run();
    void Update();

    public :

    bool Init(const GraphicsAttributes& attributes);
    unsigned int GetOpenGLVersion();
};

#endif
