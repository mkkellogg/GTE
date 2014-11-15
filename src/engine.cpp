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
#include "global/global.h"
#include "util/time.h"
#include "ui/debug.h"

Engine * Engine::theInstance = NULL;

Engine::Engine()
{
	engineObjectManager = NULL;
	graphicsEngine = NULL;
	animationManager = NULL;
	callbacks = NULL;
}

Engine::~Engine()
{
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
	engineObjectManager = new EngineObjectManager();
	ASSERT(engineObjectManager != NULL, "Engine::Init -> Unable to create engine object manager.", false);

	// TODO: add switch to detect correct type for platform
	// for now, only support OpenGL
	graphicsEngine = new GraphicsGL();
	ASSERT(graphicsEngine != NULL, "Engine::Init -> Unable to allocate graphics enigne.", false);

	bool graphicsInitSuccess = graphicsEngine->Init(graphicsAtrributes);
	ASSERT(graphicsInitSuccess == true, "Engine::Init -> Unable to initialize graphics engine.", false);

	animationManager = new AnimationManager();
	ASSERT(animationManager != NULL, "Engine::Init -> Unable to create animation manager.", false);

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

	// update timer before rendering scene so that calls to Time::GetDeltaTime() within
	// _instanceCallbacks->OnUpdate reflect rendering time
	Time::Update();

	graphicsEngine->RenderScene();
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

