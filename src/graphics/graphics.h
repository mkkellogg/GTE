#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

class Graphics;

#include "shader/shader.h"
#include "render/material.h"
#include "geometry/transform.h"
#include "render/mesh3Drenderer.h"
#include "view/viewsystem.h"

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

    Material * activeMaterial;
    ViewSystem * viewSystem;

    Graphics();
    virtual ~Graphics();

    public :

    static Graphics * Instance();

    virtual Material * CreateMaterial();
    virtual void DestroyMaterial(Material * material);

    virtual void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle);

    virtual void ActivateMaterial(Material * material);
    Material * GetActiveMaterial() const;
};

#endif
