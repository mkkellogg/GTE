#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "graphics.h"
#include "global/global.h"
#include "graphicsGL.h"
#include "shader/shader.h"
#include "render/material.h"
#include "geometry/transform.h"
#include "render/mesh3Drenderer.h"
#include "render/rendermanager.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"


Graphics * Graphics::theInstance = NULL;

Graphics::~Graphics()
{

}

Graphics::Graphics()
{
	activeMaterial= NULL;
	renderManager = new RenderManager(this, EngineObjectManager::Instance());
	screenDescriptor = NULL;
}

Graphics * Graphics::Instance()
{
	//TODO: make thread-safe & add double checked locking
    if(theInstance == NULL)
    {
        // TODO: add switch to detect correct type for platform
        // for now, only support OpenGL
        theInstance = new GraphicsGL();
    }

    return theInstance;
}

GraphicsCallbacks::~GraphicsCallbacks()
{

}

void Graphics::Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks, const char * windowTitle)
{

}

void Graphics::ActivateMaterial(Material * material)
{
	activeMaterial = material;
}

Material * Graphics::GetActiveMaterial() const
{
	return activeMaterial;
}

RenderManager * Graphics::GetRenderManager()
{
	return renderManager;
}

ScreenDescriptor * Graphics::GetScreenDescriptor() const
{
	return screenDescriptor;
}


