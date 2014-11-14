#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "graphics/graphics.h"
#include "graphics/graphicsGL.h"
#include "graphics/animation/animationmanager.h"
#include "global/global.h"
#include "util/time.h"
#include "ui/debug.h"

Engine * Engine::theInstance = NULL;

Engine::Engine()
{
	graphicsEngine = NULL;
}

Engine::~Engine()
{

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

bool Engine::Init(EngineCallbacks * callbacks)
{
	// TODO: add switch to detect correct type for platform
	// for now, only support OpenGL
	graphicsEngine = new GraphicsGL();
	ASSERT(graphicsEngine != NULL, "Engine::Init -> Unable to allocate graphics enigne.", false);

	this->callbacks = callbacks;

	if(callbacks!=NULL)callbacks->OnInit();

	return true;
}

void Engine::Update()
{
	graphicsEngine->PreProcessScene();

	if(callbacks!=NULL)callbacks->OnUpdate();
	graphicsEngine->Update();
	AnimationManager::Instance()->Update();

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
	if(graphicsEngine != NULL)
	{
		graphicsEngine->Run();
	}

	Quit();
}

Graphics * Engine::GetGraphicsEngine()
{
	return graphicsEngine;
}

