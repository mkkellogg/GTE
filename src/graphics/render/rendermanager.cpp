#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rendermanager.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "ui/debug.h"
#include "base/intmask.h"
#include "graphics/graphics.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/view/camera.h"
#include <vector>
#include "util/datastack.h"
#include "global/global.h"
#include "global/constants.h"

RenderManager::RenderManager(Graphics * graphics, EngineObjectManager * objectManager)
{
	this->graphics = graphics;
	this->objectManager = objectManager;
	viewTransformStack = new DataStack<float>(Constants::MaxObjectRecursionDepth, 16);
	if(!viewTransformStack->Init())
	{
		SAFE_DELETE(viewTransformStack);
	}
	modelTransformStack = new DataStack<float>(Constants::MaxObjectRecursionDepth, 16);
	if(!modelTransformStack->Init())
	{
		SAFE_DELETE(modelTransformStack);
	}
}

RenderManager::~RenderManager()
{
	SAFE_DELETE(viewTransformStack);
	SAFE_DELETE(modelTransformStack);
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

void RenderManager:: PushTransformData(const Transform * transform, DataStack<float> * transformStack)
{
	transformStack->Push(transform->GetMatrix()->GetDataPtr());
}

void RenderManager::PopTransformData(Transform * transform, DataStack<float> * transformStack)
{
	float * data = transformStack->Pop();
	Matrix4x4 * mat = const_cast<Matrix4x4 *>(transform->GetMatrix());
	if(data != NULL)mat->SetTo(data);
}

int RenderManager::RenderDepth(DataStack<float> * transformStack)
{
	if(transformStack == NULL)return Constants::MaxObjectRecursionDepth;
	return transformStack->GetEntryCount();

}

void RenderManager::RenderAll()
{
	Transform cameraModelView;
	const SceneObject * sceneRoot = objectManager->GetSceneRoot();
	RenderAll(const_cast<SceneObject*>(sceneRoot), &cameraModelView);
}

void RenderManager::RenderAll(SceneObject * parent, Transform * viewTransform)
{
	Transform viewInverse;
	Transform identity;

	if(RenderDepth(viewTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(std::vector<SceneObject *>::size_type i = 0; i != parent->GetChildrenCount(); i++)
	{
		SceneObject * child = parent->GetChildAt(i);

		if(child != NULL)
		{
			PushTransformData(viewTransform, viewTransformStack);
			viewTransform->TransformBy(child->GetTransform());
			Camera * camera = child->GetCamera();
			if(camera != NULL)
			{
				ClearBuffersForCamera(camera);
				const SceneObject * sceneRoot = objectManager->GetSceneRoot();

				identity.SetIdentity();
				viewInverse.SetTo(viewTransform);
				viewInverse.Invert();
				RenderScene(const_cast<SceneObject*>(sceneRoot),&identity, &viewInverse, camera);
			}

			RenderAll(child, viewTransform);
			PopTransformData(viewTransform, viewTransformStack);
		}
		else
		{
			Debug::PrintError("RenderManager::RenderAll -> NULL scene object encountered.");
		}
	}
}

void RenderManager::RenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera)
{
	Transform modelView;

	if(RenderDepth(modelTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(std::vector<SceneObject *>::size_type i = 0; i != parent->GetChildrenCount(); i++)
	{
		SceneObject * obj = parent->GetChildAt(i);

		if(obj != NULL && obj->GetTransform() != NULL)
		{
			PushTransformData(modelTransform, modelTransformStack);
			modelTransform->TransformBy(obj->GetTransform());
			Mesh3DRenderer * renderer = obj->GetRenderer3D();
			if(renderer != NULL)
			{
				Material * m = renderer->GetMaterial();
				if(m != NULL)
				{
					graphics->ActivateMaterial(m);

					modelView.SetTo(modelTransform);
					modelView.PreTransformBy(viewTransformInverse);

					graphics->SendStandardUniformsToShader(&modelView, camera->GetProjectionTransform());
					renderer->Render();
				}
				else
				{
					Debug::PrintError("RenderManager::RenderScene -> renderer has NULL material.");
				}
			}

			RenderScene(obj, modelTransform, viewTransformInverse,camera);
			PopTransformData(modelTransform,modelTransformStack);
		}
		else
		{
			Debug::PrintError("RenderManager::RenderScene -> NULL scene object encountered.");
		}
	}
}
