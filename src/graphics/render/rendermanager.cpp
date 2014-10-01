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
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"

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

	lightCount = 0;
	cameraCount = 0;

	viewTransformStack = NULL;
	modelTransformStack = NULL;
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
 * Initialize. Return false if initialization false, true if it succeeds.
 */
bool RenderManager::Init()
{
	viewTransformStack = new DataStack<float>(Constants::MaxObjectRecursionDepth, 16);
	if(!viewTransformStack->Init())
	{
		Debug::PrintError("RenderManager::Init -> unable to allocate view transform stack.");
		return false;
	}

	modelTransformStack = new DataStack<float>(Constants::MaxObjectRecursionDepth, 16);
	if(!modelTransformStack->Init())
	{
		Debug::PrintError("RenderManager::Init -> unable to allocate model transform stack.");
		SAFE_DELETE(viewTransformStack);
		return false;
	}

	return true;
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
	lightCount = 0;
	cameraCount = 0;

	Transform cameraModelView;
	const SceneObject * sceneRoot = objectManager->GetSceneRoot();
	if(sceneRoot == NULL)
	{
		Debug::PrintError("RenderManager::RenderAll -> sceneRoot is NULL.");
		return;
	}

	// gather information about the cameras & lights in the scene
	ProcessScene(const_cast<SceneObject*>(sceneRoot), &cameraModelView);

	// render the scene from the perspective of each camera found in ProcessScene()
	for(unsigned int i=0; i < cameraCount; i ++)
	{
		RenderSceneFromCamera(i);
	}
}

/*
 * Recursively visits each object in the scene that is reachable from 'parent'. Whenever an object
 * that contains a Camera component, store that camera in [sceneCameras]. Later, the entire scene is
 * rendered from the perspective of each camera in that list via RenderSceneFromCamera().
 *
 * The transforms of each scene object are concatenated as progress moves down the scene object tree
 * and passed to the current invocation via 'viewTransform', since they will ultimately form position
 * from which the scene is rendered. The transform stored in 'viewTransform' is passed to RenderScene();
 */
void RenderManager::ProcessScene(SceneObject * parent, Transform * viewTransform)
{
	Transform viewInverse;
	Transform identity;

	// enforce max recursion depth
	if(RenderDepth(viewTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(unsigned int i = 0; i < parent->GetChildrenCount(); i++)
	{
		SceneObject * child = parent->GetChildAt(i);

		if(child != NULL && child->IsActive())
		{
			// save the existing view transform
			PushTransformData(viewTransform, viewTransformStack);

			// concatenate the current view transform with that of the current scene object
			viewTransform->TransformBy(child->GetLocalTransform());

			Camera * camera = child->GetCamera();
			if(camera != NULL && cameraCount < MAX_CAMERAS)
			{
				// we invert the viewTransform because the viewTransform is really moving the world
				// relative to the camera, rather than moving the camera in the world
				viewInverse.SetTo(viewTransform);
				viewInverse.Invert();

				// add a scene camera from which to render the scene
				sceneCameras[cameraCount].transform.SetTo(&viewInverse);
				sceneCameras[cameraCount].component = camera;

				cameraCount++;
			}

			LightRef light = child->GetLight();
			if(light.IsValid() && lightCount < MAX_LIGHTS)
			{
				// add a scene light
				sceneLights[lightCount].transform.SetTo(viewTransform);
				sceneLights[lightCount].component = light.GetPtr();

				lightCount++;
			}

			// continue recursion through child object
			ProcessScene(child, viewTransform);

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
 * Render the entire scene from the perspective of a single camera. Uses [cameraIndex]
 * as an index into the array of cameras that has been found by processing the scene
 * [sceneCameras].
 */
void RenderManager::RenderSceneFromCamera(unsigned int cameraIndex)
{
	if(cameraIndex >= cameraCount)
	{
		Debug::PrintError("RenderManager::RenderSceneFromCamera -> cameraIndex out of bounds");
		return;
	}

	Camera * camera = dynamic_cast<Camera *>(sceneCameras[cameraIndex].component);

	NULL_CHECK_RTRN(camera,"RenderManager::RenderSceneFromCamera -> camera in NULL");

	// clear the appropriate render buffers this camera
	ClearBuffersForCamera(camera);
	const SceneObject * sceneRoot = objectManager->GetSceneRoot();

	Transform identity;
	identity.SetIdentity();

	// render the scene using the view transform of the current camera
	ForwardRenderScene(const_cast<SceneObject*>(sceneRoot),&identity, &(sceneCameras[cameraIndex].transform), camera);
}

/*
 * This method recursively visits all objects in the scene that are reachable from [parent] and renders
 * them from the perspective of [viewTransformInverse], which the inverse of the view transform.
 * The reason the inverse is used is because on the GPU side of things the view transform is used to move
 * the world relative to the camera, rather than move the camera in the world.
 *
 * The transforms of each scene are concatenated as progress moves down the scene object tree and passed
 * to the current invocation via [modelTransform].
 *
 * This method uses a forward-rendering approach. Each mesh is rendered once for each light and the output from
 * each pass is combined with the others using additive blending.
 */
void RenderManager::ForwardRenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera)
{
	Transform modelView;

	renderedObjects.clear();

	// enforce max recursion depth
	if(RenderDepth(modelTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	// loop through each child scene object
	for(unsigned int i = 0; i < parent->GetChildrenCount(); i++)
	{
		SceneObject * child = parent->GetChildAt(i);

		if(child == NULL ||  child->GetLocalTransform() == NULL)
		{
			Debug::PrintError("RenderManager::ForwardRenderScene -> NULL scene object encountered.");
		}
		else if(child->IsActive())
		{
			// save existing model transform
			PushTransformData(modelTransform, modelTransformStack);

			// concatenate the current model transform with that of the current scene object
			modelTransform->TransformBy(child->GetLocalTransform());

			// check if current scene object has a mesh & renderer
			Mesh3DRenderer * renderer = child->GetRenderer3D();

			if(renderer != NULL)
			{
				Mesh3D * mesh = renderer->GetMesh();

				if(mesh == NULL)
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> renderer returned NULL mesh.");
				}
				else if(mesh->GetSubMeshCount() != renderer->GetSubRendererCount())
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> Sub mesh count does not match sub renderer count!.");
				}
				else if(renderer->GetMaterialCount() <= 0)
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> renderer has no materials.");
				}
				else
				{
					unsigned int materialIndex = 0;
					for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
					{
						Material * currentMaterial = renderer->GetMaterial(materialIndex);
						if(currentMaterial == NULL)
						{
							Debug::PrintError("RenderManager::ForwardRenderScene -> NULL material encountered.");
							continue;
						}

						SubMesh3DRenderer * subRenderer = renderer->GetSubRenderer(i);
						if(subRenderer == NULL)
						{
							Debug::PrintError("RenderManager::ForwardRenderScene -> NULL sub renderer encountered.");
							continue;
						}

						SubMesh3D * subMesh = mesh->GetSubMesh(i);
						if(subMesh == NULL)
						{
							Debug::PrintError("RenderManager::ForwardRenderScene -> NULL sub mesh encountered.");
							continue;
						}

						modelView.SetTo(modelTransform);
						// concatenate modelTransform with inverted viewTransform
						modelView.PreTransformBy(viewTransformInverse);

						// activate the material, which will switch the GPU's active shader to
						// the one associated with the material
						ActivateMaterial(currentMaterial);
						// pass concatenated modelViewTransform and projection transforms to shader
						SendTransformUniformsToShader(modelTransform, &modelView, camera->GetProjectionTransform());
						SendCustomUniformsToShader();

						// loop through each active light and render sub mesh for that light, if in range
						for(unsigned int l = 0; l < lightCount; l++)
						{
							if(sceneLights[l].component == NULL)
							{
								Debug::PrintError("RenderManager::ForwardRenderScene -> sceneLights[l].component is NULL");
								continue;
							}

							Light * light = dynamic_cast<Light *>(sceneLights[l].component);
							if(light == NULL)
							{
								Debug::PrintError("RenderManager::ForwardRenderScene -> light is NULL");
								continue;
							}

							// if this sub mesh has already been rendered by this camera, then we want to use
							// additive blending to combine it with the output from other lights. Otherwise
							// turn off blending and render.
							bool rendered = renderedObjects[subMesh->GetObjectID()];
							if(rendered)
							{
								graphics->EnableBlending(true);
								graphics->SetBlendingFunction(BlendingProperty::One,BlendingProperty::One);
							}
							else
							{
								graphics->EnableBlending(false);
							}

							Point3 lightPosition;
							sceneLights[l].transform.GetMatrix()->Transform(&lightPosition);

							Transform full;

							// get the full transform of the scene object, including those of all ancestors
							child->GetFullTransform(&full);

							// check if this mesh should be culled from this light.
							if(!ShouldCullFromLight(*light, lightPosition, full, *subMesh))
							{
								// send light data to the active shader
								currentMaterial->SendLightToShader(light, &lightPosition);
								// render the current mesh
								subRenderer->Render();
								// flag the current mesh as being rendered (at least once)
								renderedObjects[subMesh->GetObjectID()] = true;
							}
						}

						// Advance material index. Renderer can have any number of materials > 0; it does not have to match
						// the number of sub meshes. If the end of the material array is reached, loop back to the beginning.
						materialIndex++;
						if(materialIndex >= renderer->GetMaterialCount())
						{
							materialIndex = 0;
						}
					}
				}
			}

			// continue recursion through child
			ForwardRenderScene(child, modelTransform, viewTransformInverse,camera);

			// restore previous modelTransform
			PopTransformData(modelTransform,modelTransformStack);
		}
	}
}

/*
 * Check if [mesh] should be rendered with [light], based
 * on its distance from [lightPosition].
 */
bool RenderManager::ShouldCullFromLight(Light& light, Point3& lightPosition, Transform& fullTransform, SubMesh3D& mesh)
{
	switch(mesh.GetLightCullType())
	{
		case LightCullType::None:
			return false;
		break;
		case LightCullType::SphereOfInfluence:
			return ShouldCullBySphereOfInfluence(light, lightPosition, fullTransform, mesh);
		break;
		case LightCullType::Tiled:
			return ShouldCullByTile(light, lightPosition, fullTransform, mesh);
		break;
		default:
			return false;
		break;
	}

	return false;
}

/*
 * Cull light based on distance of [mesh] from [light]. Each mesh has
 * a sphere of influence based on maximum distance of the mesh's vertices from the mesh's center. If that
 * sphere does not intersect with the sphere that is formed by the light's range, then the light should
 * be culled from the meshes.
 */
bool RenderManager::ShouldCullBySphereOfInfluence(Light& light, Point3& lightPosition, Transform& fullTransform, SubMesh3D& mesh)
{
	// get the maximum distances from mesh center along each axis
	Vector3 soiX = *(mesh.GetSphereOfInfluenceX());
	Vector3 soiY = *(mesh.GetSphereOfInfluenceY());
	Vector3 soiZ = *(mesh.GetSphereOfInfluenceZ());

	// transform each distance vector by the full transform of the scene
	// object that contains [mesh]
	fullTransform.GetMatrix()->Transform(&soiX);
	fullTransform.GetMatrix()->Transform(&soiY);
	fullTransform.GetMatrix()->Transform(&soiZ);

	// get length of each transformed vector
	float xMag = soiX.QuickMagnitude();
	float yMag = soiY.QuickMagnitude();
	float zMag = soiZ.QuickMagnitude();

	// find maximum distance, which will be used as the radius for
	// the sphere of influence
	float meshMag = xMag;
	if(yMag > meshMag)meshMag = yMag;
	if(zMag > meshMag)meshMag = zMag;

	Vector3 toLight;
	Point3 meshCenter = *(mesh.GetCenter());
	fullTransform.GetMatrix()->Transform(&meshCenter);

	// get the distance from the light to the mesh's center
	Point3::Subtract(&lightPosition, &meshCenter, &toLight);

	// if the distance from the mesh's center to the light is bigger
	// than the radius of the sphere of influence + the light's range,
	// the the mesh should be culled for the light.
	if(toLight.QuickMagnitude() > meshMag + light.GetRange())return true;

	return false;
}

/*
 * Tile-based culling - needs to be implemented!
 */
bool RenderManager::ShouldCullByTile(Light& light, Point3& lightPosition, Transform& fullTransform, SubMesh3D& mesh)
{
	return false;
}

/*
 * Send the ModelView matrix in [modelView] and Projection matrix in [projection] to the active shader.
 * The binding information stored in the active material holds the shader variable locations for these matrices.
 */
void RenderManager::SendTransformUniformsToShader(const Transform * model, const Transform * modelView, const Transform * projection)
{
	if(activeMaterial != NULL)
	{
		Shader * shader = activeMaterial->GetShader();
		if(shader != NULL)
		{
			Transform mvpTransform;
			mvpTransform.TransformBy(modelView);
			mvpTransform.TransformBy(projection);

			activeMaterial->SendModelMatrixToShader(model->GetMatrix());
			activeMaterial->SendModelViewMatrixToShader(modelView->GetMatrix());
			activeMaterial->SendProjectionMatrixToShader(projection->GetMatrix());
			activeMaterial->SendMVPMatrixToShader(mvpTransform.GetMatrix());
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
		activeMaterial->SendAllSetUniformsToShader();
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

