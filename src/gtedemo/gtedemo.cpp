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
class CustomEngineCallbacks: public EngineCallbacks
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

	void OnQuit(){}
	virtual ~CustomEngineCallbacks(){}
};

/*
 * Entry point for the GTE Demo. This method is minimal; its only purpose
 * is to initialize and start the engine.
 */
int main(int argc, char** argv)
{
	// instance CustomEngineCallbacks to handle engine events
	CustomEngineCallbacks engineCallbacks;

	// specify basic graphics attributes
	GraphicsAttributes graphicsAttributes;
	graphicsAttributes.WindowWidth = 1280;
	graphicsAttributes.WindowHeight = 800;
	graphicsAttributes.WindowTitle = "GTE Test";

	// initialize the engine
	bool initSuccess = Engine::Init(&engineCallbacks, graphicsAttributes);

	if(initSuccess)
	{
		// start the engine
		Engine::Start();
	}
	else
	{
		Debug::PrintError("Error occurred while initializing engine.");
		return EXIT_FAILURE;
	}

	// shutdown the engine
	Engine::ShutDown();
	return EXIT_SUCCESS;
}

