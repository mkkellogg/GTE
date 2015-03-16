#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "object/engineobjectmanager.h"
#include "graphics/graphics.h"
#include "graphics/graphicsGL.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/render/rendermanager.h"
#include "input/inputmanager.h"
#include "input/inputmanagerGL.h"
#include "error/errormanager.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"

// set singleton instance to NULL by default
Engine * Engine::theInstance = NULL;

/*
 * Default constructor
 */
Engine::Engine()
{
	engineObjectManager = NULL;
	graphicsEngine = NULL;
	renderManager = NULL;
	animationManager = NULL;
	inputManager = NULL;
	errorManager = NULL;
	callbacks = NULL;

	initialized = false;
	firstFrameEntered = false;
}

/*
 * Clean-up
 */
Engine::~Engine()
{
	SAFE_DELETE(errorManager);
	SAFE_DELETE(inputManager);
	SAFE_DELETE(animationManager);
	SAFE_DELETE(graphicsEngine);
	SAFE_DELETE(engineObjectManager);
}

EngineCallbacks::~EngineCallbacks()
{

}

/*
 * Singleton accessor
 */
Engine * Engine::Instance()
{
	//TODO: make thread-safe & add double checked locking
    if(theInstance == NULL)
    {
        theInstance = new Engine();
    }

    return theInstance;
}

/*
 * Initialization method. This method is private, but is accessible via a call to the public
 * initialization method Init(). This method is responsible for creating and initializing each
 * of the core components of the engine.
 *
 * [callbacks] - An instance of EngineCallbacks that implements methods to be called for the
 * 				 various life-cycle events of the engine.
 *
 * [graphicsAttributes] - Describes the settings that will be used to initialize the graphics
 * 						  engine component
 */
bool Engine::_Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAttributes)
{
	// The error manager must be initialized before any other engine component so that errors
	// during initialization of those components can be reported
	errorManager = new ErrorManager();
	ASSERT(errorManager != NULL, "Engine::Init -> Unable to create error manager.", false);

	engineObjectManager = new EngineObjectManager();
	ASSERT(engineObjectManager != NULL, "Engine::Init -> Unable to create engine object manager.", false);

	bool engineObjectManagerInitSuccess = engineObjectManager->Init();
	ASSERT(engineObjectManagerInitSuccess == true, "Engine::Init -> Unable to initialize engine object manager.", false);

	// TODO: add switch to detect correct type for platform
	// for now, only support OpenGL
	graphicsEngine = new GraphicsGL();
	ASSERT(graphicsEngine != NULL, "Engine::Init -> Unable to allocate graphics enigne.", false);

	bool graphicsInitSuccess = graphicsEngine->Init(graphicsAttributes);
	ASSERT(graphicsInitSuccess == true, "Engine::Init -> Unable to initialize graphics engine.", false);

	renderManager = new RenderManager();
	ASSERT(renderManager != NULL, "Engine::Init -> Unable to allocate render manager", false);

	bool renderInitSuccess = renderManager->Init();
	ASSERT(renderInitSuccess == true, "Engine::Init -> Unable to initialize render manager", false);

	// This portion of the initialization of the engine object manager must be called
	// after the graphics engine is initialized
	bool initShadersSuccess = engineObjectManager->InitBuiltinShaders();
	ASSERT(initShadersSuccess == true, "Engine::Init -> Could not initiliaze built-in shaders", false);

	animationManager = new AnimationManager();
	ASSERT(animationManager != NULL, "Engine::Init -> Unable to create animation manager.", false);

	// TODO: add switch to detect correct type for platform
	// for now, only support OpenGL
	inputManager = new InputManagerGL();
	ASSERT(inputManager != NULL, "Engine::Init -> Unable to create input manager.", false);

	bool inputInitSuccess = inputManager->Init();
	ASSERT(inputInitSuccess, "Engine::Init -> Unable to initialize input manager.", false);

	this->callbacks = callbacks;

	initialized = true;

	return true;
}

/*
 * Public initialization method. This method first makes sure the singleton instance has
 * been created successfully, then calls the private initialization function _Init();
 */
bool Engine::Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes)
{
	Engine * engine = Engine::Instance();
	ASSERT(engine != NULL, "Engine::Init -> Unable retrieve Engine instance.", false);

	return engine->_Init(callbacks, graphicsAtrributes);
}

/*
 * This method is the core update method of the engine and drives all engine events.
 */
void Engine::Update()
{
	if(!firstFrameEntered)
	{
		if(callbacks!=NULL)callbacks->OnInit();
		Time::Update();
		firstFrameEntered = true;
	}
	graphicsEngine->Update();
	animationManager->Update();
	inputManager->Update();
	if(callbacks!=NULL)callbacks->OnUpdate();
	graphicsEngine->PreProcessScene();
	renderManager->PreProcessScene();
	if(callbacks!=NULL)callbacks->OnPreRender();
	graphicsEngine->RenderScene();
	Time::Update();
}

/*
 * This method is called when the engine's update loop stops.
 */
void Engine::Quit()
{
	if(callbacks!=NULL)callbacks->OnQuit();
}

/*
 * Public method to start the engine update loop. It first checks to make sure the
 * singleton instance has been created and properly initialized, and then calls the
 * private method _Start().
 */
void Engine::Start()
{
	Engine * engine = Engine::Instance();
	ASSERT_RTRN(engine != NULL, "Engine::Start retrieve Engine instance.");

	ASSERT_RTRN(engine->IsInitialized(), "Engine::Start -> Engine instance is not initialized.");

	engine->_Start();

}

/*
 * Private method to kick of the engine update loop.
 */
void Engine::_Start()
{
	if(graphicsEngine != NULL)graphicsEngine->Start();
	Quit();
}

/*
 * This is a method that is called externally to shutdown the engine
 * and deallocate any resources.
 */
void Engine::ShutDown()
{
	Engine * engine = Engine::Instance();
	ASSERT_RTRN(engine != NULL, "Engine::ShutDown -> Unable retrieve Engine instance.");

	SAFE_DELETE(theInstance);
	theInstance = NULL;
}

/*
 * Has the Init() method been called successfully?
 */
bool Engine::IsInitialized() const
{
	return initialized;
}

/*
 * Access the EngineObjectManager component.
 */
EngineObjectManager * Engine::GetEngineObjectManager()
{
	return engineObjectManager;
}

/*
 * Access the Graphics component.
 */
Graphics * Engine::GetGraphicsEngine()
{
	return graphicsEngine;
}

/*
 * Access the RenderManager component.
 */
RenderManager * Engine::GetRenderManager()
{
	return renderManager;
}

/*
 * Access the AnimationManager component.
 */
AnimationManager * Engine::GetAnimationManager()
{
	return animationManager;
}

/*
 * Access the InputManager component.
 */
InputManager * Engine::GetInputManager()
{
	return inputManager;
}

/*
 * Access the ErrorManager component.
 */
ErrorManager * Engine::GetErrorManager()
{
	return errorManager;
}

