#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <memory>
#include "engine.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "graphics/animation/bone.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendertarget.h"
#include "graphics/render/material.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "debug/debug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "util/time.h"
#include "gtedemo.h"
#include "gtedemo/game.h"

Game * game = NULL;
class CustomEngineCallbacks: public EngineCallbacks
{
	public:

	CustomEngineCallbacks(){}
	void OnInit(){}
	void OnUpdate()
	{
		game->Update();
	}

	void OnQuit(){}
	virtual ~CustomEngineCallbacks(){}
};

int main(int argc, char** argv)
{
	CustomEngineCallbacks engineCallbacks;

	game = new Game();

	GraphicsAttributes graphicsAttributes;
	graphicsAttributes.WindowWidth = 1280;
	graphicsAttributes.WindowHeight = 800;
	graphicsAttributes.WindowTitle = "GTE Test";

	bool initSuccess = Engine::Init(&engineCallbacks, graphicsAttributes);

	if(initSuccess)
	{
		game->Init();
		Engine::Start();
	}
	else
	{
		Debug::PrintError("Error occurred while initializing engine.");
	}

	Engine::ShutDown();
	return EXIT_SUCCESS;
}

