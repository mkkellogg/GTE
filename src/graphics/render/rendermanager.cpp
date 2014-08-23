#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rendermanager.h"
#include "object/sceneobject.h"
#include "ui/debug.h"
#include "graphics/graphics.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/view/camera.h"
#include <vector>

RenderManager::RenderManager(Graphics * graphics)
{
	this->graphics = graphics;
}

RenderManager::~RenderManager()
{

}

void RenderManager::AddMeshRenderer(SceneObject * obj)
{
	if(obj != NULL)
	{
		renderers.push_back(obj);
	}
	else
	{
		Debug::PrintError("RenderManager::AddMeshRenderer() -> attempted to add NULL renderer object.");
	}
}

void RenderManager::RemoveMeshRenderer(SceneObject * obj)
{

}

void RenderManager::RenderAll(const Camera * camera)
{
	for(std::vector<SceneObject *>::size_type i = 0; i != renderers.size(); i++)
	{
		SceneObject * obj = renderers[i];

		if(obj != NULL)
		{
			Mesh3DRenderer * renderer = obj->GetRenderer3D();
			if(renderer != NULL)
			{
				graphics->SendStandardUniformsToShader(camera);
				renderer->Render();
			}
			else
			{
				Debug::PrintError("RenderManager::RenderAll -> a scene object contained NULL Mesh3DRenderer.");
			}
		}
		else
		{
			Debug::PrintError("RenderManager::RenderAll -> renderers contains NULL scene object.");
		}
	}
}
