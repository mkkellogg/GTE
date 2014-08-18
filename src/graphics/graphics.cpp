#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "graphics.h"
#include "graphicsGL.h"
#include "gte.h"

Graphics * Graphics::theInstance = NULL;

Graphics::~Graphics()
{

}

Graphics::Graphics() : activeMaterial(NULL)
{

}

Graphics * Graphics::Instance()
{
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

Material * Graphics::CreateMaterial()
{
	return new Material();
}

void Graphics::DestroyMaterial(Material * material)
{
	delete material;
}

void Graphics::ActivateMaterial(Material * material)
{
	activeMaterial = material;
}

Material * Graphics::GetActiveMaterial()
{
	return activeMaterial;
}

