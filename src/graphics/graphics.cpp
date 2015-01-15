#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "engine.h"
#include "graphics.h"
#include "global/global.h"
#include "graphicsGL.h"
#include "shader/shader.h"
#include "render/material.h"
#include "geometry/transform.h"
#include "render/submesh3Drenderer.h"
#include "render/rendermanager.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "global/global.h"
#include "debug/debug.h"
#include "util/time.h"

Graphics::~Graphics()
{

}

Graphics::Graphics()
{
	renderManager = NULL;
    fpsTime =0;
	frames =0;
	currentFPS = 0.0;
}

bool Graphics::Init(const GraphicsAttributes& attributes)
{
	EngineObjectManager * engineObjectManager = Engine::Instance()->GetEngineObjectManager();
	renderManager = new RenderManager();
	ASSERT(renderManager != NULL, "Graphics::Init -> Unable to allocate render manager", false);

	bool renderInitSuccess = renderManager->Init();
	if(!renderInitSuccess)return false;

	return true;
}

void Graphics::PreProcessScene()
{

}

void Graphics::Update()
{
	UpdateFPS();
}

void Graphics::RenderScene()
{

}

void Graphics::UpdateFPS()
{
	fpsTime += Time::GetDeltaTime();
	frames++;
	if(fpsTime >= 1)
	{
		currentFPS = (float)frames/fpsTime;
		//printf("fps: %f\n", currentFPS);
		fpsTime = 0;
		frames = 0;
	}
}

bool Graphics::Run()
{
	return true;
}

void Graphics::ActivateMaterial(MaterialRef material)
{
	activeMaterial = material;
	material->ResetVerificationState();
}

MaterialRef Graphics::GetActiveMaterial() const
{
	return activeMaterial;
}

RenderManager * Graphics::GetRenderManager()
{
	return renderManager;
}

float Graphics::GetCurrentFPS()
{
	return currentFPS;
}

const GraphicsAttributes& Graphics::GetAttributes() const
{
	return attributes;
}


