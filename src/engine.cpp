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

namespace GTE
{
	// set singleton instance to NULL by default
	Engine * Engine::theInstance = NULL;

	/*
	 * Default constructor
	 */
	Engine::Engine()
	{
		engineObjectManager = NULL;
		graphicsSystem = NULL;
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
		SAFE_DELETE(inputManager);
		SAFE_DELETE(animationManager);
		SAFE_DELETE(engineObjectManager);
		SAFE_DELETE(renderManager);
		SAFE_DELETE(graphicsSystem);
		SAFE_DELETE(errorManager);
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
		if (theInstance == NULL)
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
	Bool Engine::_Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAttributes)
	{
		// The error manager must be initialized before any other engine component so that errors
		// during initialization of those components can be reported
		errorManager = new ErrorManager();
		ASSERT(errorManager != NULL, "Engine::Init -> Unable to create error manager.");

		engineObjectManager = new EngineObjectManager();
		ASSERT(engineObjectManager != NULL, "Engine::Init -> Unable to create engine object manager.");

		Bool engineObjectManagerInitSuccess = engineObjectManager->Init();
		ASSERT(engineObjectManagerInitSuccess == true, "Engine::Init -> Unable to initialize engine object manager.");

		// TODO: add switch to detect correct type for platform
		// for now, only support OpenGL
		graphicsSystem = new GraphicsGL();
		ASSERT(graphicsSystem != NULL, "Engine::Init -> Unable to allocate graphics engine.");

		Bool graphicsInitSuccess = graphicsSystem->Init(graphicsAttributes);
		ASSERT(graphicsInitSuccess == true, "Engine::Init -> Unable to initialize graphics engine.");

		renderManager = new RenderManager();
		ASSERT(renderManager != NULL, "Engine::Init -> Unable to allocate render manager");

		Bool renderInitSuccess = renderManager->Init();
		ASSERT(renderInitSuccess == true, "Engine::Init -> Unable to initialize render manager");

		// This portion of the initialization of the engine object manager must be called
		// after the graphics engine is initialized
		Bool initShadersSuccess = engineObjectManager->InitBuiltinShaders();
		ASSERT(initShadersSuccess == true, "Engine::Init -> Could not initiliaze built-in shaders");

		animationManager = new AnimationManager();
		ASSERT(animationManager != NULL, "Engine::Init -> Unable to create animation manager.");

		// TODO: add switch to detect correct type for platform
		// for now, only support OpenGL
		inputManager = new InputManagerGL();
		ASSERT(inputManager != NULL, "Engine::Init -> Unable to create input manager.");

		Bool inputInitSuccess = inputManager->Init();
		ASSERT(inputInitSuccess, "Engine::Init -> Unable to initialize input manager.");

		this->callbacks = callbacks;

		initialized = true;

		return true;
	}

	/*
	 * Public initialization method. This method first makes sure the singleton instance has
	 * been created successfully, then calls the private initialization function _Init();
	 */
	Bool Engine::Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes)
	{
		Engine * engine = Engine::Instance();
		ASSERT(engine != NULL, "Engine::Init -> Unable retrieve Engine instance.");

		return engine->_Init(callbacks, graphicsAtrributes);
	}

	/*
	 * This method is the core update method of the engine and drives all engine events.
	 */
	void Engine::Update()
	{
		if (!firstFrameEntered)
		{
			if (callbacks != NULL)callbacks->OnInit();
			Time::Update();
			firstFrameEntered = true;
		}

		graphicsSystem->Update();
		animationManager->Update();
		inputManager->Update();
		if (callbacks != NULL)callbacks->OnUpdate();
		renderManager->PreProcessScene();
		if (callbacks != NULL)callbacks->OnPreRender();
		renderManager->RenderScene();
		graphicsSystem->PostRender();

		Time::Update();
	}

	/*
	 * This method is called when the engine's update loop stops.
	 */
	void Engine::Quit()
	{
		if (callbacks != NULL)callbacks->OnQuit();
	}

	/*
	 * Public method to start the engine update loop. It first checks to make sure the
	 * singleton instance has been created and properly initialized, and then calls the
	 * private method _Start().
	 */
	void Engine::Start()
	{
		Engine * engine = Engine::Instance();
		ASSERT(engine != NULL, "Engine::Start retrieve Engine instance.");

		NONFATAL_ASSERT(engine->IsInitialized(), "Engine::Start -> Engine instance is not initialized.", true);

		engine->_Start();

	}

	/*
	 * Private method to kick of the engine update loop.
	 */
	void Engine::_Start()
	{
		if (graphicsSystem != NULL)graphicsSystem->Start();
		Quit();
	}

	/*
	 * This is a method that is called externally to shutdown the engine
	 * and deallocate any resources.
	 */
	void Engine::ShutDown()
	{
		Engine * engine = Engine::Instance();
		ASSERT(engine != NULL, "Engine::ShutDown -> Unable retrieve Engine instance.");

		SAFE_DELETE(theInstance);
		theInstance = NULL;
	}

	/*
	 * Has the Init() method been called successfully?
	 */
	Bool Engine::IsInitialized() const
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
	 * Access the the interface to the lower-level graphics system.
	 */
	Graphics * Engine::GetGraphicsSystem()
	{
		return graphicsSystem;
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
}

