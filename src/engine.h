#ifndef _GTE_ENGINE_H_
#define _GTE_ENGINE_H_

//forward declarations
class Graphics;
class EngineObjectManager;
class AnimationManager;
class InputManager;
class ErrorManager;

#include <string>
#include "object/enginetypes.h"
#include "graphics/graphics.h"

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

	EngineObjectManager * engineObjectManager;
	Graphics * graphicsEngine;
	AnimationManager * animationManager;
	InputManager * inputManager;
	ErrorManager * errorManager;
	EngineCallbacks * callbacks;

	Engine();
    ~Engine();

    void Update();
    void Quit();
    bool _Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes);
    void _Start();

    public :

    static Engine * Instance();
    static void ShutDown();
    static bool Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes);
    static void Start();

    EngineObjectManager * GetEngineObjectManager();
    Graphics * GetGraphicsEngine();
    AnimationManager * GetAnimationManager();
    InputManager * GetInputManager();
    ErrorManager * GetErrorManager();
};

#endif
