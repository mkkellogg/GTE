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
#include "view/viewsystem.h"


Graphics * Graphics::theInstance = NULL;

Graphics::~Graphics()
{

}

Graphics::Graphics()
{
	activeMaterial= NULL;
	viewSystem = NULL;
	renderManager = new RenderManager(this);
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
	viewSystem = new ViewSystem(this);
}

Material * Graphics::CreateMaterial()
{
	return new Material();
}

void Graphics::DestroyMaterial(Material * material)
{
	SAFE_DELETE(material);
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

