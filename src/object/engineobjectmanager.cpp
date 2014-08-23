#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobjectmanager.h"
#include "sceneobject.h"
#include "graphics/graphics.h"
#include "graphics/attributes.h"
#include "graphics/object/mesh3D.h"

EngineObjectManager * EngineObjectManager::theInstance = NULL;

EngineObjectManager * EngineObjectManager::Instance()
{
	//TODO: make thread-safe & add double checked locking
    if(theInstance == NULL)
    {
        theInstance = new EngineObjectManager();
    }

    return theInstance;
}

EngineObjectManager::EngineObjectManager()
{

}

EngineObjectManager::~EngineObjectManager()
{

}

SceneObject * EngineObjectManager::CreateSceneObject()
{
	SceneObject *obj = new SceneObject();
	sceneObjects.push_back(obj);
	return obj;
}

Mesh3D * EngineObjectManager::CreateMesh3D()
{
	return new Mesh3D();
}

Mesh3DRenderer * EngineObjectManager::CreateMesh3DRenderer()
{
	Graphics * graphics = Graphics::Instance();
	return graphics->CreateMeshRenderer();
}

Shader * EngineObjectManager::CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath)
{
	Graphics * graphics = Graphics::Instance();
	return graphics->CreateShader(vertexSourcePath,fragmentSourcePath);
}
