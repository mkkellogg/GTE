#ifndef _ENGINE_H_
#define _ENGINE_H_

//forward declarations
class Graphics;

#include <string>
#include "object/enginetypes.h"

class EngineCallbacks
{
    public:

    virtual void OnInit() = 0;
    virtual void OnQuit() = 0;
    virtual void OnUpdate() = 0;
    virtual ~EngineCallbacks();
};

class Engine
{
	friend class Graphics;
	friend class GraphicsGL;

	static Engine * theInstance;

	Graphics * graphicsEngine;
	EngineCallbacks * callbacks;

	Engine();
    ~Engine();

    void Update();
    void Quit();

    public :

    static Engine * Instance();

    bool Init(EngineCallbacks * callbacks);
    void Start();

    Graphics * GetGraphicsEngine();
};

#endif
