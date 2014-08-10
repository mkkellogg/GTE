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
    shaderManager = new ShaderManager();
}

Graphics::Graphics()
{
    delete shaderManager;
}

ShaderManager * Graphics::GetShaderManager()
{
    return shaderManager;
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

