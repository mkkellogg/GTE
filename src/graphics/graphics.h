#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "shader/shader.h"
#include "vertexattrbuffer.h"

class Graphics;

class GraphicsCallbacks
{
    public:

    virtual void OnInit(Graphics * graphics) = 0;
    virtual void OnQuit(Graphics * graphics) = 0;
    virtual void OnUpdate(Graphics * graphics) = 0;
    virtual ~GraphicsCallbacks();
};

class Graphics
{
    protected:

    static Graphics * theInstance;

    Graphics();
    virtual ~Graphics();

    public :

    static Graphics * Instance();

    virtual Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)  const  = 0;
    virtual VertexAttrBuffer * CreateVertexAttrBuffer() const  = 0;
    virtual void DestroyShader(Shader * shader) const = 0;
    virtual void DestroyVertexAttrBuffer(VertexAttrBuffer * buffer) const = 0;

    virtual void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle) = 0;
};

#endif
