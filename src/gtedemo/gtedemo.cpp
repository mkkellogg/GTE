#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <memory>
#include "engine.h"
#include "debug/gtedebug.h"
#include "graphics/graphics.h"
#include "gtedemo.h"
#include "gtedemo/game.h"

/*
 * Custom implementation of EngineCallbacks to handle GTE engine events.
 */
class CustomEngineCallbacks : public GTE::EngineCallbacks
{
	Game * game = NULL;

	public:

	CustomEngineCallbacks(){}

	void OnInit()
	{
		// instantiate the main Game instance
		game = new Game();

		// initialize the main game instance
		game->Init();
	}

	void OnUpdate()
	{
		// pass the Update event to the main Game instance
		game->Update();
	}

	 void OnPreRender()
	 {
		 // pass the OnPreRender event to the main Game instance
		 game->OnPreRender();
	 }

	void OnQuit()
	{
		game->OnQuit();
	}
	virtual ~CustomEngineCallbacks(){}
};

/*
 * Entry point for the GTE Demo. This method is minimal; its only purpose
 * is to initialize and start the engine.
 */
int main(GTE::Int32 argc, char** argv)
{
	// instance CustomEngineCallbacks to handle engine events
	CustomEngineCallbacks engineCallbacks;

	// specify basic graphics attributes
	GTE::GraphicsAttributes graphicsAttributes;
	graphicsAttributes.WindowWidth = 1280;
	graphicsAttributes.WindowHeight = 800;
	graphicsAttributes.WindowTitle = "GTE Test";
	graphicsAttributes.AAMethod = GTE::AntialiasingMethod::MSAAx4;
	graphicsAttributes.WaitForVSync = false;

	// initialize the engine
	bool initSuccess = GTE::Engine::Init(&engineCallbacks, graphicsAttributes);

	if(initSuccess)
	{
		// start the engine
		GTE::Engine::Start();
	}
	else
	{
		GTE::Debug::PrintError("Error occurred while initializing engine.");
		return EXIT_FAILURE;
	}

	// shutdown the engine
	GTE::Engine::ShutDown();
	return EXIT_SUCCESS;
}

