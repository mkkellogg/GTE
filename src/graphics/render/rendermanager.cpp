#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rendermanager.h"
#include "geometry/transform.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "ui/debug.h"
#include "base/intmask.h"
#include "graphics/graphics.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/view/camera.h"
#include <vector>

RenderManager::RenderManager(Graphics * graphics, EngineObjectManager * objectManager)
{
	this->graphics = graphics;
	this->objectManager = objectManager;
}

RenderManager::~RenderManager()
{

}

void RenderManager::ClearBuffersForCamera(const Camera * camera) const
{
	unsigned int clearBufferMask = camera->GetClearBufferMask();

	GLbitfield glClearMask = 0;
	if(IntMask::IsBitSetForMask(clearBufferMask, (unsigned int)RenderBufferType::Color))
		glClearMask |= GL_COLOR_BUFFER_BIT;
	if(IntMask::IsBitSetForMask(clearBufferMask, (unsigned int)RenderBufferType::Depth))
		glClearMask |= GL_DEPTH_BUFFER_BIT;

	glClear(glClearMask);
}

void RenderManager::RenderAll()
{
	Transform modelView;

	const SceneObject * sceneRoot = objectManager->GetSceneRoot();
	for(std::vector<SceneObject *>::size_type i = 0; i != sceneRoot->GetChildrenCount(); i++)
	{
		SceneObject * obj = sceneRoot->GetChildAt(i);

		if(obj != NULL)
		{
			Camera * camera = obj->GetCamera();
			if(camera != NULL)
			{
				ClearBuffersForCamera(camera);
				RenderScene(&modelView, camera);
			}
		}
		else
		{
			Debug::PrintError("RenderManager::RenderAll -> NULL scene object encountered.");
		}
	}
}

void RenderManager::RenderScene(Transform *modelViewTransform, Camera * camera)
{
	const SceneObject * sceneRoot = objectManager->GetSceneRoot();
	for(std::vector<SceneObject *>::size_type i = 0; i != sceneRoot->GetChildrenCount(); i++)
	{
		SceneObject * obj = sceneRoot->GetChildAt(i);

		if(obj != NULL)
		{
			Mesh3DRenderer * renderer = obj->GetRenderer3D();
			if(renderer != NULL)
			{
				Material * m = renderer->GetMaterial();
				if(m != NULL)
				{
					graphics->ActivateMaterial(m);
					graphics->SendStandardUniformsToShader(modelViewTransform, camera->GetProjectionTransform());
					renderer->Render();
				}
				else
				{
					Debug::PrintError("RenderManager::RenderScene -> renderer has NULL material.");
				}
			}
		}
		else
		{
			Debug::PrintError("RenderManager::RenderScene -> NULL scene object encountered.");
		}
	}
}
