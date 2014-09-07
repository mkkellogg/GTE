#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobjectmanager.h"
#include "sceneobject.h"
#include "graphics/graphics.h"
#include "graphics/shader/shader.h"
#include "graphics/light/light.h"
#include "graphics/render/material.h"
#include "graphics/stdattributes.h"
#include "graphics/object/mesh3D.h"
#include "graphics/texture/texture.h"
#include "ui/debug.h"
#include "graphics/view/camera.h"

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
	sceneRoot.AddChild(obj);
	return obj;
}

Mesh3D * EngineObjectManager::CreateMesh3D(StandardAttributeSet attributes)
{
	return new Mesh3D(attributes);
}

void EngineObjectManager::DestroyMesh3D(Mesh3D * mesh)
{
	delete mesh;
}

Mesh3DRenderer * EngineObjectManager::CreateMesh3DRenderer()
{
	Graphics * graphics = Graphics::Instance();
	return graphics->CreateMeshRenderer();
}

void EngineObjectManager::DestroyMesh3DRenderer(Mesh3DRenderer * renderer)
{
	Graphics * graphics = Graphics::Instance();
	graphics->DestroyMeshRenderer(renderer);
}

Shader * EngineObjectManager::CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath)
{
	Graphics * graphics = Graphics::Instance();
	Shader * shader = graphics->CreateShader(vertexSourcePath,fragmentSourcePath);
	return shader;
}

void EngineObjectManager::DestroyShader(Shader * shader)
{
	Graphics * graphics = Graphics::Instance();
	graphics->DestroyShader(shader);
}

Texture * EngineObjectManager::CreateTexture(const char * sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Graphics::Instance();
	return graphics->CreateTexture(sourcePath, attributes);
}

Material * EngineObjectManager::CreateMaterial(const char *name, Shader * shader)
{
	Material * m = new Material(name);
	bool initSuccess = m->Init(shader);
	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(Shader *) -> could not Init material");
		delete m;
		return NULL;
	}
	return m;
}

Material * EngineObjectManager::CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath)
{

	Shader * shader = CreateShader(shaderVertexSourcePath, shaderFragmentSourcePath);
	if(shader == NULL)return NULL;

	Material * m = new Material(name);
	bool initSuccess = m->Init(shader);

	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(const char *, const char *) -> could not Init material");
		delete m;
		return NULL;
	}
	return m;
}

void EngineObjectManager::DestroyMaterial(Material * material)
{
	Shader * shader = material->GetShader();
	if(shader != NULL)
	{
		DestroyShader(shader);
	}

	delete material;
}

Camera * EngineObjectManager::CreateCamera()
{
	Graphics * graphics = Graphics::Instance();
	return new Camera(graphics);
}

void EngineObjectManager::DestroyCamera(Camera * camera)
{
	delete camera;
}

Light * EngineObjectManager::CreateLight()
{
	Light * light = new Light();
	return light;
}

void EngineObjectManager::DestroyLight(Light * light)
{
	delete light;
}

const SceneObject * EngineObjectManager::GetSceneRoot() const
{
	return &sceneRoot;
}
