#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "rendermanager.h"
#include "material.h"
#include "graphics/shader/shader.h"
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

/*
 * Single constructor, which requires pointers the Graphics and EngineObjectManager
 * singleton instances.
 */
RenderManager::RenderManager(Graphics * graphics, EngineObjectManager * objectManager)
{
	this->activeMaterial = NULL;
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

/*
 * Clean up
 */
RenderManager::~RenderManager()
{
	SAFE_DELETE(viewTransformStack);
	SAFE_DELETE(modelTransformStack);
}

/*
 * Look at the clear flags for a given camera and tell the graphics
 * system to clear the corresponding buffers.
 */
void RenderManager::ClearBuffersForCamera(const Camera * camera) const
{
	if(camera == NULL)
	{
		Debug::PrintError("RenderManager::ClearBuffersForCamera -> camera is NULL.");
		return;
	}
	unsigned int clearBufferMask = camera->GetClearBufferMask();
	graphics->ClearBuffers(clearBufferMask);
}

/*
 * Save a transform to the transform stack. This method is used to to save transformations
 * as the render manager progresses through the object tree that makes up the scene.
 */
void RenderManager::PushTransformData(const Transform * transform, DataStack<float> * transformStack)
{
	if(transform == NULL)
	{
		Debug::PrintError("RenderManager::PushTransformData -> transform is NULL.");
		return;
	}
	if(transformStack == NULL)
	{
		Debug::PrintError("RenderManager::PushTransformData -> transformStack is NULL.");
		return;
	}
	transformStack->Push(transform->GetMatrix()->GetDataPtr());
}

/*
 * Remove the top transform from the transform stack.
 */
void RenderManager::PopTransformData(const Transform * transform, DataStack<float> * transformStack)
{
	if(transform == NULL)
	{
		Debug::PrintError("RenderManager::PopTransformData -> transform is NULL.");
		return;
	}
	if(transformStack == NULL)
	{
		Debug::PrintError("RenderManager::PopTransformData -> transformStack is NULL.");
		return;
	}
	float * data = transformStack->Pop();
	Matrix4x4 * mat = const_cast<Matrix4x4 *>(transform->GetMatrix());
	if(data != NULL)mat->SetTo(data);
}

/*
 * Get the number of entries stored on the transform stack.
 */
int RenderManager::RenderDepth(const DataStack<float> * transformStack) const
{
	if(transformStack == NULL)return Constants::MaxObjectRecursionDepth;
	return transformStack->GetEntryCount();

}

/*
 * Kick off rendering of the entire scene. This method starts with the root object
 * in the scene and kicks off a method that renders all objects in the scene.
 */
void RenderManager::RenderAll()
{
	Transform cameraModelView;
	const SceneObject * sceneRoot = objectManager->GetSceneRoot();
	if(sceneRoot == NULL)
	{
		Debug::PrintError("RenderManager::RenderAll -> sceneRoot is NULL.");
		return;
	}
	RenderFromCameras(const_cast<SceneObject*>(sceneRoot), &cameraModelView);
}

/*
 * Recursively visits each object in the scene that is reachable from 'parent'. Whenever an object
 * that contains a Camera component is encountered, the entire scene is rendered from the perspective
 * of that camera via RenderScene().
 *
 * The transforms of each scene object are concatenated as progress moves down the scene object tree
 * and passed to the current invocation via 'viewTransform', since they will ultimately form position
 * from which the scene is rendered. The transform stored in 'viewTransform' is passed to RenderScene();
 */
void RenderManager::RenderFromCameras(SceneObject * parent, Transform * viewTransform)
{
	Transform viewInverse;
	Transform identity;

	// enforce max recursion depth
	if(RenderDepth(viewTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(std::vector<SceneObject *>::size_type i = 0; i != parent->GetChildrenCount(); i++)
	{
		SceneObject * child = parent->GetChildAt(i);

		if(child != NULL)
		{
			// save the existing view transform
			PushTransformData(viewTransform, viewTransformStack);
			// concatenate the current view transform with that of the current scene object
			viewTransform->TransformBy(child->GetTransform());
			Camera * camera = child->GetCamera();
			if(camera != NULL)
			{
				// clear the appropriate render buffers this camera
				ClearBuffersForCamera(camera);
				const SceneObject * sceneRoot = objectManager->GetSceneRoot();

				identity.SetIdentity();

				// we invert the viewTransform because the viewTransform is really moving the world
				// relative to the camera, rather than moving the camera in the world
				viewInverse.SetTo(viewTransform);
				viewInverse.Invert();

				// render the scene using the view transform of the current camera
				RenderScene(const_cast<SceneObject*>(sceneRoot),&identity, &viewInverse, camera);
			}

			// continue recursion through child object
			RenderFromCameras(child, viewTransform);

			// restore previous view transform
			PopTransformData(viewTransform, viewTransformStack);
		}
		else
		{
			Debug::PrintError("RenderManager::RenderAll -> NULL scene object encountered.");
		}
	}
}

/*
 * This method recursively visits all objects in the scene that are reachable from [parent] and renders
 * them from the perspective of [viewTransformInverse], which the inverse of the view transform.
 * The reason the inverse is used is because on the GPU side of things the view transform is used to move
 * the world relative to the camera, rather than move the camera in the world.
 *
 * The transforms of each scene are concatenated as progress moves down the scene object tree and passed
 * to the current invocation via [modelTransform].
 */
void RenderManager::RenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera)
{
	Transform modelView;

	// enforce max recursion depth
	if(RenderDepth(modelTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(std::vector<SceneObject *>::size_type i = 0; i != parent->GetChildrenCount(); i++)
	{
		SceneObject * child = parent->GetChildAt(i);

		if(child != NULL && child->GetTransform() != NULL)
		{
			// save existing model transform
			PushTransformData(modelTransform, modelTransformStack);
			// concatenate the current model transform with that of the current scene object
			modelTransform->TransformBy(child->GetTransform());
			// check if current scene object has a mesh renderer
			Mesh3DRenderer * renderer = child->GetRenderer3D();
			if(renderer != NULL)
			{
				// make sure mesh renderer has a material set
				Material * currentMaterial = renderer->GetMaterial();
				if(currentMaterial != NULL)
				{
					modelView.SetTo(modelTransform);
					// concatenate modelTransform with inverted viewTransform
					modelView.PreTransformBy(viewTransformInverse);

					// activate the material, which will switch the GPU's active shader to
					// the one associated with the material
					ActivateMaterial(currentMaterial);
					// pass concatenated modelViewTransform and projection transforms to shader
					SendTransformUniformsToShader(&modelView, camera->GetProjectionTransform());
					SendCustomUniformsToShader();
					renderer->Render(currentMaterial);
				}
				else
				{
					Debug::PrintError("RenderManager::RenderScene -> renderer has NULL material.");
				}
			}

			// continue recursion through child
			RenderScene(child, modelTransform, viewTransformInverse,camera);

			// restore previous modelTransform
			PopTransformData(modelTransform,modelTransformStack);
		}
		else
		{
			Debug::PrintError("RenderManager::RenderScene -> NULL scene object encountered.");
		}
	}
}

/*
 * Send the ModelView matrix in [modelView] and Projection matrix in [projection] to the active shader.
 * The binding information stored in the active material holds the shader variable locations for these matrices.
 */
void RenderManager::SendTransformUniformsToShader(const Transform * modelView, const Transform * projection)
{
	if(activeMaterial != NULL)
	{
		int mvMatrixLoc = activeMaterial->GetStandardUniformShaderVarLocation(Uniform::ModelViewMatrix);
		int mvpMatrixLoc = activeMaterial->GetStandardUniformShaderVarLocation(Uniform::ModelViewProjectionMatrix);
		int projectionMatrixLoc = activeMaterial->GetStandardUniformShaderVarLocation(Uniform::ProjectionMatrix);

		Shader * shader = activeMaterial->GetShader();
		if(shader != NULL)
		{
			Transform mvpTransform;
			mvpTransform.TransformBy(modelView);
			mvpTransform.TransformBy(projection);

			if(mvMatrixLoc >= 0)shader->SendUniformToShader(mvMatrixLoc, modelView->GetMatrix());
			if(mvpMatrixLoc >= 0)shader->SendUniformToShader(mvpMatrixLoc, mvpTransform.GetMatrix());
			if(projectionMatrixLoc >= 0)shader->SendUniformToShader(projectionMatrixLoc, projection->GetMatrix());
		}
		else
		{
			Debug::PrintError("RenderManager::SendTransformUniformsToShader -> material contains NULL shader.");
		}
	}
	else
	{
		Debug::PrintError("RenderManager::SendTransformUniformsToShader -> activeMaterial is NULL.");
	}
}

/*
 * Send any custom uniforms specified by the active material to the active shader
 */
void RenderManager::SendCustomUniformsToShader()
{
	if(activeMaterial != NULL)
	{
		Shader * shader = activeMaterial->GetShader();
		for(unsigned int i=0; i < activeMaterial->GetCustomUniformCount(); i++)
		{
			UniformDescriptor * desc = activeMaterial->GetCustomUniform(i);

			if(desc->Type == UniformType::Sampler)
			{
				shader->SendUniformToShader(desc->ShaderVarID, desc->SamplerData);
			}
		}
	}
	else
	{
		Debug::PrintError("RenderManager::SendCustomUniformsToShader -> activeMaterial is NULL.");
	}
}

/*
 * Activate [material], which will switch the GPU's active shader to
 * the one associated with it.
 */
void RenderManager::ActivateMaterial(Material * material)
{
	// We MUST notify the graphics system about the change in active material because other
	// components (like Mesh3DRenderer) need to know about the active material
	graphics->ActivateMaterial(material);
	this->activeMaterial = material;
}

