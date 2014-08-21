#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "graphics.h"
#include "global/global.h"
#include "graphicsGL.h"
#include "gte.h"

Graphics * Graphics::theInstance = NULL;

Graphics::~Graphics()
{
	SAFE_DELETE(modelViewTransform);
	SAFE_DELETE(projectionTransform);
}

Graphics::Graphics() : activeMaterial(NULL), modelViewTransform(Transform::CreteIdentityTransform()), projectionTransform(Transform::CreteIdentityTransform())
{

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

