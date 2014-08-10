#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "shader/shadermanager.h"

class Graphics;

class GraphicsCallbacks
{
    public:

    virtual void OnInit(Graphics * graphics) = 0;
    virtual void OnQuit(Graphics * graphics) = 0;
    virtual ~GraphicsCallbacks();
};

class Graphics
{
    protected:
    static Graphics * theInstance;
    Graphics();

    ShaderManager * shaderManager;

    public :

    static Graphics * Instance();

    ShaderManager * GetShaderManager();

    virtual ~Graphics();
    virtual void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks) = 0;
};

#endif
