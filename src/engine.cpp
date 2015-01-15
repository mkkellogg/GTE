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
#include "input/inputmanager.h"
#include "input/inputmanagerGL.h"
#include "error/errormanager.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/debug.h"

Engine * Engine::theInstance = NULL;

Engine::Engine()
{
	engineObjectManager = NULL;
	graphicsEngine = NULL;
	animationManager = NULL;
	inputManager = NULL;
	errorManager = NULL;
	callbacks = NULL;
}

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

Engine * Engine::Instance()
{
	//TODO: make thread-safe & add double checked locking
    if(theInstance == NULL)
    {
        theInstance = new Engine();
    }

    return theInstance;
}

bool Engine::_Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes)
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

	bool graphicsInitSuccess = graphicsEngine->Init(graphicsAtrributes);
	ASSERT(graphicsInitSuccess == true, "Engine::Init -> Unable to initialize graphics engine.", false);

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

	if(callbacks!=NULL)callbacks->OnInit();

	return true;
}

bool Engine::Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes)
{
	Engine * engine = Engine::Instance();
	ASSERT(engine != NULL, "Engine::Init -> Unable retrieve Engine instance.", false);

	return engine->_Init(callbacks, graphicsAtrributes);
}

void Engine::Update()
{
	graphicsEngine->PreProcessScene();
	if(callbacks!=NULL)callbacks->OnUpdate();
	graphicsEngine->Update();
	animationManager->Update();
	graphicsEngine->RenderScene();
	Time::Update();
}

void Engine::Quit()
{
	if(callbacks!=NULL)callbacks->OnQuit();
}

void Engine::Start()
{
	Engine * engine = Engine::Instance();
	ASSERT_RTRN(engine != NULL, "Engine::Start retrieve Engine instance.");
	engine->_Start();

}

void Engine::_Start()
{
	if(graphicsEngine != NULL)graphicsEngine->Run();
	Quit();
}

void Engine::ShutDown()
{
	Engine * engine = Engine::Instance();
	ASSERT_RTRN(engine != NULL, "Engine::ShutDown -> Unable retrieve Engine instance.");

	SAFE_DELETE(theInstance);
	theInstance = NULL;
}

EngineObjectManager * Engine::GetEngineObjectManager()
{
	return engineObjectManager;
}

Graphics * Engine::GetGraphicsEngine()
{
	return graphicsEngine;
}

AnimationManager * Engine::GetAnimationManager()
{
	return animationManager;
}

InputManager * Engine::GetInputManager()
{
	return inputManager;
}

ErrorManager * Engine::GetErrorManager()
{
	return errorManager;
}

