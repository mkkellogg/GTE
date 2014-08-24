#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "sceneobject.h"
#include "engineobject.h"
#include "graphics/graphics.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendermanager.h"
#include "ui/debug.h"

SceneObject::SceneObject() : EngineObject()
{
	renderer3D = NULL;
	mesh3D = NULL;
}

SceneObject::~SceneObject()
{

}

bool SceneObject::SetMeshRenderer(Mesh3DRenderer *renderer)
{
	if(renderer != NULL && renderer3D != renderer)
	{
		this->renderer3D = renderer;
		if(mesh3D != NULL)
		{
			renderer3D->UseMesh(mesh3D);
			UpdateRenderManager();
		}
	}
	else
	{
		Debug::PrintError("SceneObject::AddMeshRenderer -> attempted to add NULL renderer.");
	}
	return true;
}

bool SceneObject::SetMesh(Mesh3D *mesh)
{
	if(mesh != NULL)
	{
		this->mesh3D = mesh;
		if(renderer3D != NULL)
		{
			renderer3D->UseMesh(mesh);
		}
	}
	else
	{
		Debug::PrintError("SceneObject::AddMesh -> attempted to add NULL mesh.");
	}
	return true;
}

void SceneObject::UpdateRenderManager()
{
	Graphics * graphics = Graphics::Instance();
	RenderManager * renderManager = graphics->GetRenderManager();

	if(renderManager != NULL)
	{
		renderManager->AddMeshRenderer(this);
	}
	else
	{
		Debug::PrintError("SceneObject::UpdateRenderManager -> the render manager is NULL.");
	}
}

Mesh3DRenderer * SceneObject::GetRenderer3D()
{
	return renderer3D;
}
