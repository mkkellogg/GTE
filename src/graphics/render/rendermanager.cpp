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
#include "object/enginetypes.h"
#include "ui/debug.h"
#include "base/intmask.h"
#include "graphics/graphics.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
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
RenderManager::RenderManager(Graphics * graphics, EngineObjectManager * objectManager) : viewTransformStack(Constants::MaxObjectRecursionDepth, 1),
																						 modelTransformStack(Constants::MaxObjectRecursionDepth, 1)
{
	this->graphics = graphics;
	this->objectManager = objectManager;

	lightCount = 0;
	cameraCount = 0;
	sceneMeshCount = 0;
	cycleCount = 0;
}

/*
 * Clean up
 */
RenderManager::~RenderManager()
{

}

/*
 * Initialize. Return false if initialization false, true if it succeeds.
 */
bool RenderManager::Init()
{
	if(!viewTransformStack.Init())
	{
		Debug::PrintError("RenderManager::Init -> unable to initialize view transform stack.");
		return false;
	}

	if(!modelTransformStack.Init())
	{
		Debug::PrintError("RenderManager::Init -> unable to initialize model transform stack.");
		return false;
	}

	return true;
}

/*
 * Look at the clear flags for a given camera and tell the graphics
 * system to clear the corresponding buffers.
 */
void RenderManager::ClearBuffersForCamera(const Camera& camera) const
{
	unsigned int clearBufferMask = camera.GetClearBufferMask();
	graphics->ClearBuffers(clearBufferMask);
}

/*
 * Save a transform to the transform stack. This method is used to to save transformations
 * as the render manager progresses through the object tree that makes up the scene.
 */
void RenderManager::PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack)
{
	transformStack.Push(&transform.matrix);
}

/*
 * Remove the top transform from the transform stack.
 */
void RenderManager::PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack)
{
	ASSERT_RTRN(transformStack.GetEntryCount() > 0,"RenderManager::PopTransformData -> transformStack is empty!");

	Matrix4x4 * mat = transformStack.Pop();
	transform.SetTo(*mat);
}

/*
 * Get the number of entries stored on the transform stack.
 */
unsigned int RenderManager::RenderDepth(const DataStack<Matrix4x4>& transformStack) const
{
	return transformStack.GetEntryCount();

}

/*
 * Kick off rendering of the entire scene. This method starts with the root object
 * in the scene and kicks off a method that renders all objects in the scene.
 */
void RenderManager::RenderAll()
{
	ProcessScene();

	// render the scene from the perspective of each camera found in ProcessScene()
	for(unsigned int i=0; i < cameraCount; i ++)
	{
		RenderSceneFromCamera(i);
	}
}

/*
 * Kick off the scene processing from the root of the scene.
 */
void RenderManager::ProcessScene()
{
	lightCount = 0;
	cameraCount = 0;
	sceneMeshCount = 0;

	Transform cameraModelView;

	SceneObjectRef sceneRoot = (SceneObjectRef)objectManager->GetSceneRoot();
	ASSERT_RTRN(sceneRoot.IsValid(),"RenderManager::RenderAll -> sceneRoot is NULL.");

	// gather information about the cameras & lights in the scene
	ProcessScene(sceneRoot.GetRef(), cameraModelView);

	cycleCount++;
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
void RenderManager::ProcessScene(SceneObject& parent, Transform& aggregateTransform)
{
	Transform viewInverse;
	Transform identity;
	Transform model;
	Transform modelInverse;

	// enforce max recursion depth
	if(RenderDepth(viewTransformStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(unsigned int i = 0; i < parent.GetChildrenCount(); i++)
	{
		SceneObjectRef child = parent.GetChildAt(i);

		if(child.IsValid() && child->IsActive())
		{
			//if(cycleCount <= 0)printf("node: %s\n", child->GetName());

			// save the existing view transform
			PushTransformData(aggregateTransform, viewTransformStack);

			// concatenate the current view transform with that of the current scene object
			Transform& localTransform = child->GetLocalTransform();
			aggregateTransform.TransformBy(localTransform);

			CameraRef camera = child->GetCamera();
			if(camera.IsValid() && cameraCount < MAX_CAMERAS)
			{
				// we invert the viewTransform because the viewTransform is really moving the world
				// relative to the camera, rather than moving the camera in the world
				viewInverse.SetTo(aggregateTransform);
				viewInverse.Invert();

				// add a scene camera from which to render the scene
				sceneCameras[cameraCount].AffectorTransform.SetTo(viewInverse);
				sceneCameras[cameraCount].AffectorCamera = camera.GetPtr();

				cameraCount++;
			}

			LightRef light = child->GetLight();
			if(light.IsValid() && lightCount < MAX_LIGHTS)
			{
				// add a scene light
				sceneLights[lightCount].AffectorTransform.SetTo(aggregateTransform);
				sceneLights[lightCount].AffectorLight = light.GetPtr();

				lightCount++;
			}

			Mesh3DRendererRef meshRenderer = child->GetMesh3DRenderer();
			SkinnedMesh3DRendererRef skinnedMeshRenderer = child->GetSkinnedMesh3DRenderer();

			if(meshRenderer.IsValid() && sceneMeshCount < MAX_SCENE_MESHES)
			{
				meshObjects[sceneMeshCount] = child.GetPtr();

				model.SetTo(aggregateTransform);
				modelInverse.SetTo(model);
				modelInverse.Invert();

				for(unsigned int r = 0; r< meshRenderer->GetSubRendererCount(); r++)
				{
					SubMesh3DRendererRef subRenderer = meshRenderer->GetSubRenderer(r);
					if(subRenderer.IsValid())
					{
						subRenderer->PreRender(model.matrix, modelInverse.matrix);
					}
				}

				sceneMeshCount++;
			}


			if(skinnedMeshRenderer.IsValid() && sceneMeshCount < MAX_SCENE_MESHES)
			{
				meshObjects[sceneMeshCount] = child.GetPtr();

				model.SetTo(aggregateTransform);
				modelInverse.SetTo(model);
				modelInverse.Invert();

				for(unsigned int r = 0; r< skinnedMeshRenderer->GetSubRendererCount(); r++)
				{
					SubMesh3DRendererRef subRenderer = skinnedMeshRenderer->GetSubRenderer(r);
					if(subRenderer.IsValid())
					{
						subRenderer->PreRender(model.matrix, modelInverse.matrix);
					}
				}

				sceneMeshCount++;
			}

			child->SetProcessingTransform(aggregateTransform);

			// continue recursion through child object
			ProcessScene(child.GetRef(), aggregateTransform);

			// restore previous view transform
			PopTransformData(aggregateTransform, viewTransformStack);
		}
		else
		{
			Debug::PrintError("RenderManager::RenderAll -> NULL scene object encountered.");
		}
	}
}

/*
 * Render the entire scene from the perspective of a single camera. Uses [cameraIndex]
 * as an index into the array of cameras [sceneCameras] that has been found by processing the scene.
 */
void RenderManager::RenderSceneFromCamera(unsigned int cameraIndex)
{
	ASSERT_RTRN(cameraIndex < cameraCount,"RenderManager::RenderSceneFromCamera -> cameraIndex out of bounds");

	Camera * cameraPtr = sceneCameras[cameraIndex].AffectorCamera;
	ASSERT_RTRN(cameraPtr != NULL,"RenderManager::RenderSceneFromCamera -> camera in NULL");
	Camera& camera = *cameraPtr;

	// clear the appropriate render buffers this camera
	ClearBuffersForCamera(camera);
	SceneObjectRef sceneRoot = (SceneObjectRef)objectManager->GetSceneRoot();
	ASSERT_RTRN(sceneRoot.IsValid(),"RenderManager::RenderSceneFromCamera -> sceneRoot is NULL.");

	// render the scene using the view transform of the current camera
	Transform& cameraTransform = sceneCameras[cameraIndex].AffectorTransform;
	ForwardRenderScene(cameraTransform, camera);
}

/*
 * This method looks at each mesh that was found in the ProcessScene() method and renders each of
 * them from the perspective of [viewTransformInverse], which the inverse of the view transform.
 * The reason the inverse is used is because on the GPU side of things the view transform is used to move
 * the world relative to the camera, rather than move the camera in the world.
 *
 * This method uses a forward-rendering approach. Each mesh is rendered once for each light and the output from
 * each pass is combined with the others using additive blending.
 */
void RenderManager::ForwardRenderScene(const Transform& viewTransformInverse, const Camera& camera)
{
	renderedObjects.clear();

	graphics->SetBlendingFunction(BlendingProperty::One,BlendingProperty::One);
	// loop through each active light and render sub mesh for that light, if in range
	for(unsigned int l = 0; l < lightCount; l++)
	{
		Light * light = sceneLights[l].AffectorLight;
		if(light == NULL)
		{
			Debug::PrintError("RenderManager::ForwardRenderScene -> light is NULL");
			continue;
		}

		Point3 lightPosition;
		sceneLights[l].AffectorTransform.TransformPoint(lightPosition);

		RenderSceneForLight(*light, lightPosition, viewTransformInverse, camera);
	}
}

/*
 * Render all the meshes found in ProcessScene() for a single light [light] from the perspective of
 * [viewTransformInverse], which the inverse of the view transform.
 */
void RenderManager::RenderSceneForLight(const Light& light, const Point3& lightPosition, const Transform& viewTransformInverse, const Camera& camera)
{
	Transform modelView;
	Transform model;
	Transform modelInverse;

	for(unsigned int s = 0; s < sceneMeshCount; s++)
	{
		SceneObject * child = meshObjects[s];

		if(child == NULL)
		{
			Debug::PrintError("RenderManager::ForwardRenderScene -> NULL scene object encountered.");
		}
		else if(child->IsActive())
		{
			RenderSceneObjectMeshes(*child, light, lightPosition, viewTransformInverse, camera);
		}
	}
}

void RenderManager::RenderSceneObjectMeshes(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Transform& viewTransformInverse, const Camera& camera)
{
	Mesh3DRenderer * renderer = NULL;
	Transform modelView;
	Transform model;
	Transform modelInverse;

	// check if current scene object has a mesh & renderer
	if(sceneObject.GetMesh3DRenderer().IsValid())
	{
		renderer = sceneObject.GetMesh3DRenderer().GetPtr();
	}
	else if(sceneObject.GetSkinnedMesh3DRenderer().IsValid())
	{
		renderer = (Mesh3DRenderer *)sceneObject.GetSkinnedMesh3DRenderer().GetPtr();
	}

	if(renderer != NULL)
	{
		Mesh3DRef mesh = renderer->GetMesh();

		if(!mesh.IsValid())
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
				bool doRender = true;
				MaterialRef currentMaterial = renderer->GetMaterial(materialIndex);
				if(!currentMaterial.IsValid())
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> NULL material encountered.");
					doRender = false;
				}

				SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
				if(!subRenderer.IsValid())
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> NULL sub renderer encountered.");
					doRender = false;
				}

				SubMesh3DRef subMesh = mesh->GetSubMesh(i);
				if(!subMesh.IsValid())
				{
					Debug::PrintError("RenderManager::ForwardRenderScene -> NULL sub mesh encountered.");
					doRender = false;
				}

				if(doRender)
				{
					// get the full transform of the scene object, including those of all ancestors
					SceneObjectTransform full;
					sceneObject.InitSceneObjectTransform(&full);

					// check if this mesh should be culled from this light.
					if(!ShouldCullFromLight(light, lightPosition, full, *mesh))
					{
						model.SetTo(sceneObject.GetProcessingTransform());
						modelView.SetTo(model);
						// concatenate modelTransform with inverted viewTransform
						modelView.PreTransformBy(viewTransformInverse);

						// activate the material, which will switch the GPU's active shader to
						// the one associated with the material
						ActivateMaterial(currentMaterial);
						SendActiveMaterialUniformsToShader();
						// send light data to the active shader
						currentMaterial->SendLightToShader(&light, &lightPosition);

						// pass concatenated modelViewTransform and projection transforms to shader
						SendTransformUniformsToShader(model, modelView, camera.GetProjectionTransform());

						// if this sub mesh has already been rendered by this camera, then we want to use
						// additive blending to combine it with the output from other lights. Otherwise
						// turn off blending and render.
						bool rendered = renderedObjects[subMesh->GetObjectID()];
						if(rendered)
						{
							graphics->EnableBlending(true);
						}
						else
						{
							graphics->EnableBlending(false);
						}

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
}

/*
 * Check if [mesh] should be rendered with [light], based on the distance of the center of [mesh] from [lightPosition].
 */
bool RenderManager::ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform,  const Mesh3D& mesh) const
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
 * Cull light based on distance of center of [mesh] from [light]. Each mesh has
 * a sphere of influence based on maximum distance of the mesh's vertices from the mesh's center. If that
 * sphere does not intersect with the sphere that is formed by the light's range, then the light should
 * be culled from the meshes.
 */
bool RenderManager::ShouldCullBySphereOfInfluence(const Light& light, const Point3& lightPosition, const Transform& fullTransform,  const Mesh3D& mesh) const
{
	// get the maximum distances from mesh center along each axis
	Vector3 soiX = mesh.GetSphereOfInfluenceX();
	Vector3 soiY = mesh.GetSphereOfInfluenceY();
	Vector3 soiZ = mesh.GetSphereOfInfluenceZ();

	// transform each distance vector by the full transform of the scene
	// object that contains [mesh]
	fullTransform.TransformVector(soiX);
	fullTransform.TransformVector(soiY);
	fullTransform.TransformVector(soiZ);

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
	Point3 meshCenter = mesh.GetCenter();
	fullTransform.TransformPoint(meshCenter);

	// get the distance from the light to the mesh's center
	Point3::Subtract(lightPosition, meshCenter, toLight);

	// if the distance from the mesh's center to the light is bigger
	// than the radius of the sphere of influence + the light's range,
	// the the mesh should be culled for the light.
	if(toLight.QuickMagnitude() > meshMag + light.GetRange())return true;

	return false;
}

/*
 * TODO: (Eventually) - Implement tile-base culling.
 */
bool RenderManager::ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const
{
	return false;
}

/*
 * Send the ModelView matrix in [modelView] and Projection matrix in [projection] to the active shader.
 * The binding information stored in the active material holds the shader variable locations for these matrices.
 */
void RenderManager::SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& projection)
{
	MaterialRef activeMaterial = graphics->GetActiveMaterial();
	ASSERT_RTRN(activeMaterial.IsValid(),"RenderManager::SendTransformUniformsToShader -> activeMaterial is NULL.");

	ShaderRef shader = activeMaterial->GetShader();
	ASSERT_RTRN(shader.IsValid(),"RenderManager::SendTransformUniformsToShader -> material contains NULL shader.");

	Transform mvpTransform;
	mvpTransform.TransformBy(modelView);
	mvpTransform.TransformBy(projection);

	activeMaterial->SendModelMatrixToShader(&model.matrix);
	activeMaterial->SendModelViewMatrixToShader(&modelView.matrix);
	activeMaterial->SendProjectionMatrixToShader(&projection.matrix);
	activeMaterial->SendMVPMatrixToShader(&mvpTransform.matrix);
}

/*
 * Send any custom uniforms specified by the active material to the active shader
 */
void RenderManager::SendActiveMaterialUniformsToShader()
{
	MaterialRef activeMaterial = graphics->GetActiveMaterial();
	ASSERT_RTRN(activeMaterial.IsValid(),"RenderManager::SendCustomUniformsToShader -> activeMaterial is not valid.");
	activeMaterial->SendAllSetUniformsToShader();
}

/*
 * Activate [material], which will switch the GPU's active shader to
 * the one associated with it.
 */
void RenderManager::ActivateMaterial(MaterialRef material)
{
	// We MUST notify the graphics system about the change in active material because other
	// components (like Mesh3DRenderer) need to know about the active material
	graphics->ActivateMaterial(material);
}

