#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <vector>
#include "rendermanager.h"
#include "material.h"
#include "geometry/transform.h"
#include "geometry/quaternion.h"
#include "geometry/sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "object/enginetypes.h"
#include "engine.h"
#include "base/intmask.h"
#include "graphics/shader/shader.h"
#include "graphics/graphics.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "util/datastack.h"
#include "global/global.h"
#include "global/constants.h"
#include "ui/debug.h"

/*
 * Single constructor, which requires pointers the Graphics and EngineObjectManager
 * singleton instances.
 */
RenderManager::RenderManager() : viewTransformStack(Constants::MaxObjectRecursionDepth, 1),
								 modelTransformStack(Constants::MaxObjectRecursionDepth, 1)
{
	this->graphics = Engine::Instance()->GetGraphicsEngine();
	this->objectManager = Engine::Instance()->GetEngineObjectManager();

	lightCount = 0;
	ambientLightCount = 0;
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

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	shadowVolumeMaterial = objectManager->CreateMaterial("ShadowVolumeMaterial", "resources/builtin/shadowvolume.vertex.shader","resources/builtin/shadowvolume.fragment.shader");
	ASSERT(shadowVolumeMaterial.IsValid(), "RenderManager::Init -> Unable to create shadow volume material.", false);

	return true;
}

/*
 * Look at the clear flags for a given camera and tell the graphics
 * system to clear the corresponding buffers.
 */
void RenderManager::ClearBuffersForCamera(const Camera& camera) const
{
	unsigned int clearBufferMask = camera.GetClearBufferMask();
	graphics->ClearRenderBuffers(clearBufferMask);
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
 * Kick off rendering of the entire scene. This method first processes the scene hierarchy and
 * stores a list of all cameras, lights, and meshes in the scene. After that it renders the scene
 * from the perspective of each camera.
 */
void RenderManager::RenderAll()
{
	// find all the cameras, lights, and meshes in the scene
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
	ambientLightCount = 0;
	cameraCount = 0;
	sceneMeshCount = 0;

	Transform cameraModelView;

	SceneObjectRef sceneRoot = (SceneObjectRef)objectManager->GetSceneRoot();
	ASSERT_RTRN(sceneRoot.IsValid(),"RenderManager::ProcessScene -> sceneRoot is NULL.");

	// gather information about the cameras, lights, and renderable meshes in the scene
	ProcessScene(sceneRoot.GetRef(), cameraModelView);

	cycleCount++;
}

/*
 * ProcessScene:
 *
 * Recursively visits each object in the scene that is reachable from [parent]. The transforms of each
 * scene object are concatenated as progress moves down the scene object tree and passed to the current
 * invocation via [aggregateTransform]. These aggregate transforms are saved to each SceneObject via
 * SceneObject::SetAggregateTransform().
 *
 * Additionally, for each SceneObject that is visited during this search, this method saves references to:
 *
 *   1. Camera objects -> saved to [sceneCameras]
 *   2. Light objects -> saved to [sceneAmbientLights] or [sceneLights]
 *   3. Mesh3DRenderer and SkinnedMesh3DRenderer objects -> saved to [sceneMeshObjects]
 *
 * For each of the above objects, a copy of the aggregate transform of the containing SceneObject is saved along
 * with a reference to the object.
 *
 * Later, the entire scene is rendered from the perspective of each camera in that list via RenderSceneFromCamera().
 *
 * For each Mesh3DRender and SkinnedMesh3DRenderer that is visited, the method loops through each sub-renderer
 * and calls the SubMesh3DRenderer::PreRender() Method. For skinned meshes, this method will perform the
 * vertex skinning transformation.
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

		if(!child.IsValid())
		{
			Debug::PrintError("RenderManager::ProcessScene -> NULL scene object encountered.");
			continue;
		}

		if(child->IsActive())
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
				// we invert the viewTransform because at the shader level, the viewTransform is
				// really moving the world relative to the camera, rather than moving the camera
				// in the world
				viewInverse.SetTo(aggregateTransform);
				viewInverse.Invert();

				// add a scene camera from which to render the scene
				sceneCameras[cameraCount].AffectorTransform.SetTo(viewInverse);
				sceneCameras[cameraCount].AffectorCamera = camera.GetPtr();

				cameraCount++;
			}

			LightRef light = child->GetLight();
			if(light.IsValid())
			{
				if(light->GetType() == LightType::Ambient)
				{
					if(ambientLightCount < MAX_LIGHTS)
					{
						sceneAmbientLights[ambientLightCount].AffectorTransform.SetTo(aggregateTransform);
						sceneAmbientLights[ambientLightCount].AffectorLight = light.GetPtr();
						ambientLightCount++;
					}
				}
				else
				{
					if(lightCount < MAX_LIGHTS)
					{
						// add a scene light
						sceneLights[lightCount].AffectorTransform.SetTo(aggregateTransform);
						sceneLights[lightCount].AffectorLight = light.GetPtr();
						lightCount++;
					}
				}
			}

			Mesh3DRendererRef meshRenderer = child->GetMesh3DRenderer();
			SkinnedMesh3DRendererRef skinnedMeshRenderer = child->GetSkinnedMesh3DRenderer();

			if(meshRenderer.IsValid() && sceneMeshCount < MAX_SCENE_MESHES)
			{
				sceneMeshObjects[sceneMeshCount] = child.GetPtr();

				model.SetTo(aggregateTransform);
				modelInverse.SetTo(model);
				modelInverse.Invert();

				// for each sub-renderer, call the PreRender() method
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
				sceneMeshObjects[sceneMeshCount] = child.GetPtr();

				model.SetTo(aggregateTransform);
				modelInverse.SetTo(model);
				modelInverse.Invert();

				// for each sub-renderer, call the PreRender() method
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

			// save the aggregate/global/world transform
			child->SetAggregateTransform(aggregateTransform);

			// continue recursion through child object
			ProcessScene(child.GetRef(), aggregateTransform);

			// restore previous view transform
			PopTransformData(aggregateTransform, viewTransformStack);
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
	// clear the list of objects that have been rendered at least once. this list is used to
	// determine if blending should be turned on or off. if an object is being rendered for the
	// first time, blending should be off; otherwise it should be on.
	renderedObjects.clear();

	// we have not yet rendered any ambient lights
	bool renderedAmbient = false;

	// loop through each ambient light and render the scene for that light
	for(unsigned int l = 0; l < ambientLightCount; l++)
	{
		Light * light = sceneAmbientLights[l].AffectorLight;
		if(light == NULL)
		{
			Debug::PrintWarning("RenderManager::ForwardRenderScene -> ambient light is NULL");
			continue;
		}

		RenderSceneForLight(*light, sceneLights[l].AffectorTransform, viewTransformInverse, camera, l > 0);
		renderedAmbient = true;
	}

	// loop through each regular light and render scene for that light
	for(unsigned int l = 0; l < lightCount; l++)
	{
		Light * light = sceneLights[l].AffectorLight;
		if(light == NULL)
		{
			Debug::PrintWarning("RenderManager::ForwardRenderScene -> light is NULL");
			continue;
		}

		// if [renderedAmbient] is true, the RenderSceneForLight() method will have the depth buffer already
		// set up for shadow rendering
		RenderSceneForLight(*light, sceneLights[l].AffectorTransform, viewTransformInverse, camera, renderedAmbient);
	}
}

/*
 * Render all the meshes found in ProcessScene() for a single light [light] from the perspective of
 * [viewTransformInverse], which is the inverse of the view transform.
 *
 * This method performs two passes:
 *
 * Pass 0: If [light] is not ambient, render shadow volumes for all meshes in the scene for [light] into the stencil buffer.
 * Pass 1: Perform actual rendering of all meshes in the scene for [light]. If [light] is not ambient, this pass will
 *         exclude screen pixels that are hidden from [light] based on the stencil buffer contents from pass 0. Is [light]
 *         is ambient, then this pass will perform a standard render of all meshes in the scene.
 */
void RenderManager::RenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& viewTransformInverse, const Camera& camera, bool depthBufferComplete)
{
	Transform modelView;
	Transform model;
	Transform modelInverse;

	Point3 lightPosition;
	lightFullTransform.TransformPoint(lightPosition);
	Transform lightInverse;
	lightInverse.SetTo(lightFullTransform);
	lightInverse.Invert();

	for(int pass = 0; pass < 2; pass++)
	{
		if(pass == 0) // shadow volume pass
		{
			// check if this light can cast shadows; if not we skip this pass
			if(light.GetShadowsEnabled() && light.GetType() != LightType::Ambient)
				graphics->EnterRenderMode(RenderMode::ShadowVolumeRender);
			else
				continue;
		}
		else if(pass == 1) // normal rendering pass
		{
			// check if this light can cast shadows, if not do standard (shadow-less) rendering
			if(light.GetShadowsEnabled() && light.GetType() != LightType::Ambient)
				graphics->EnterRenderMode(RenderMode::StandardWithShadowVolumeTest);
			else if(light.GetType() == LightType::Ambient)
				graphics->EnterRenderMode(RenderMode::Standard);
		}

		// loop through each mesh-containing SceneObject in [sceneMeshObjects]
		for(unsigned int s = 0; s < sceneMeshCount; s++)
		{
			SceneObject * child = sceneMeshObjects[s];

			if(child == NULL)
			{
				Debug::PrintWarning("RenderManager::RenderSceneForLight -> NULL scene object encountered.");
				continue;
			}

			if(!child->IsActive())continue;

			Mesh3DRenderer * renderer = NULL;

			// check if current SceneObject has a mesh & renderer
			if(child->GetMesh3DRenderer().IsValid())renderer =child->GetMesh3DRenderer().GetPtr();
			else if(child->GetSkinnedMesh3DRenderer().IsValid())renderer = (Mesh3DRenderer *)child->GetSkinnedMesh3DRenderer().GetPtr();
			else
			{
				Debug::PrintWarning("RenderManager::RenderSceneForLight -> Could not find renderer for mesh.");
				continue;
			}

			Mesh3DRef mesh = renderer->GetMesh();

			if(!mesh.IsValid())
			{
				Debug::PrintWarning("RenderManager::RenderSceneForLight -> Invalid mesh encountered.");
				continue;
			}

			// copy the full transform of the scene object, including those of all ancestors
			SceneObjectTransform full;
			child->InitSceneObjectTransform(&full);

			// check if this mesh should be culled from this light.
			if( light.GetType() == LightType::Directional || light.GetType() == LightType::Ambient || !ShouldCullFromLight(light, lightPosition, full, *mesh))
			{
				if(pass == 0) // shadow volume pass
				{
					if(mesh->GetCastShadows())
					{
						RenderShadowVolumesForSceneObject(*child, light, lightPosition, lightFullTransform, lightInverse, viewTransformInverse, camera);
					}
				}
				else if(pass == 1) // normal rendering pass
				{
					RenderSceneObjectMeshes(*child, light, lightPosition, viewTransformInverse, camera);
				}
			}
		}
	}
}

/*
 * Render the meshes attached to [sceneObject] for [light].
 */
void RenderManager::RenderSceneObjectMeshes(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Transform& viewTransformInverse, const Camera& camera)
{
	Mesh3DRenderer * renderer = NULL;
	Transform modelViewProjection;
	Transform modelView;
	Transform model;
	Transform modelInverse;

	// check if [sceneObject] has a mesh & renderer
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

		ASSERT_RTRN(mesh.IsValid(),"RenderManager::RenderSceneObjectMeshes -> renderer returned NULL mesh.");
		ASSERT_RTRN(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(),"RenderManager::RenderSceneObjectMeshes -> Sub mesh count does not match sub renderer count!.");
		ASSERT_RTRN(renderer->GetMaterialCount() > 0,"RenderManager::RenderSceneObjectMeshes -> renderer has no materials.");

		unsigned int materialIndex = 0;

		// loop through each sub-renderer and render its mesh(es)
		for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
		{
			MaterialRef currentMaterial = renderer->GetMaterial(materialIndex);
			SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
			SubMesh3DRef subMesh = mesh->GetSubMesh(i);

			ASSERT_RTRN(currentMaterial.IsValid(),"RenderManager::RenderSceneObjectMeshes -> NULL material encountered.")
			ASSERT_RTRN(subRenderer.IsValid(), "RenderManager::RenderSceneObjectMeshes -> NULL sub renderer encountered.");
			ASSERT_RTRN(subMesh.IsValid(), "RenderManager::RenderSceneObjectMeshes -> NULL sub mesh encountered.");

			model.SetTo(sceneObject.GetAggregateTransform());

			// concatenate model transform with inverted view transform, and then with
			// the camera's projection transform.
			modelView.SetTo(model);
			modelView.PreTransformBy(viewTransformInverse);
			modelViewProjection.SetTo(modelView);
			modelViewProjection.PreTransformBy(camera.GetProjectionTransform());

			// activate the material, which will switch the GPU's active shader to
			// the one associated with the material
			ActivateMaterial(currentMaterial);
			// send uniforms set for the new material to its shader
			SendActiveMaterialUniformsToShader();
			// send light data to the active shader
			currentMaterial->SendLightToShader(&light, &lightPosition, NULL);
			// pass concatenated modelViewTransform and projection transforms to shader
			SendTransformUniformsToShader(model, modelView, camera.GetProjectionTransform(), modelViewProjection);

			// if this sub mesh has already been rendered by this camera, then we want to use
			// additive blending to combine it with the output from other lights. Otherwise
			// turn off blending and render.
			bool rendered = renderedObjects[subMesh->GetObjectID()];
			if(rendered)
			{
				graphics->SetBlendingEnabled(true);
				graphics->SetBlendingFunction(BlendingProperty::One,BlendingProperty::One);
			}
			else
			{
				graphics->SetBlendingEnabled(false);
			}

			// render the current mesh
			subRenderer->Render();

			// flag the current mesh as being rendered (at least once)
			renderedObjects[subMesh->GetObjectID()] = true;

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

/*
 * Render the shadow volumes for the meshes attached to [sceneObject] for [light]. This essentially means altering the
 * stencil buffer to reflect areas of the rendered scene that are shadowed from [light] by the meshes attached to [sceneObject].
 */
void RenderManager::RenderShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Transform& lightTransform,
														 const Transform& lightTransformInverse, const Transform& viewTransformInverse, const Camera& camera)
{
	Mesh3DRenderer * renderer = NULL;
	Transform modelViewProjection;
	Transform modelView;
	Transform model;
	Transform modelInverse;

	// check if [sceneObject] has a mesh & renderer
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

		if(mesh->GetSubMeshCount() != renderer->GetSubRendererCount())
		{
			printf("renderer->GetSubRendererCount(): %d\n", renderer->GetSubRendererCount());
			printf("mesh->GetSubMeshCount(): %d\n",mesh->GetSubMeshCount());
		}

		ASSERT_RTRN(mesh.IsValid(),"RenderManager::RenderShadowVolumesForSceneObject -> renderer returned NULL mesh.");
		ASSERT_RTRN(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(),"RenderManager::RenderShadowVolumesForSceneObject -> Sub mesh count does not match sub renderer count!.");
		ASSERT_RTRN(renderer->GetMaterialCount() > 0,"RenderManager::RenderShadowVolumesForSceneObject -> renderer has no materials.");

		// loop through each sub-renderer and render the shadow volume for its mesh(es)
		for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
		{
			MaterialRef currentMaterial = renderer->GetMaterial(0);
			SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
			SubMesh3DRef subMesh = mesh->GetSubMesh(i);

			ASSERT_RTRN(shadowVolumeMaterial.IsValid(),"RenderManager::RenderShadowVolumesForSceneObject -> NULL material encountered.")
			ASSERT_RTRN(subRenderer.IsValid(), "RenderManager::RenderShadowVolumesForSceneObject -> NULL sub renderer encountered.");
			ASSERT_RTRN(subMesh.IsValid(), "RenderManager::RenderShadowVolumesForSceneObject -> NULL sub mesh encountered.");

			// calculate model transform and inverse model transform
			model.SetTo(sceneObject.GetAggregateTransform());
			modelInverse.SetTo(model);
			modelInverse.Invert();

			// calculate the position and/or direction of [light]
			// in the mesh's local space
			Point3 modelLocalLightPos = lightPosition;
			Vector3 modelLocalLightDir = light.GetDirection();
			modelInverse.TransformPoint(modelLocalLightPos);
			modelInverse.TransformVector(modelLocalLightDir);

			// build special MVP transform for rendering shadow volumes
			BuildShadowVolumeMVPTransform(light, mesh->GetCenter(), model, modelLocalLightPos, modelLocalLightDir, camera, viewTransformInverse, modelViewProjection);

			// activate the material, which will switch the GPU's active shader to
			// the one associated with the material
			ActivateMaterial(shadowVolumeMaterial);
			// pass special shadow volume model-view-matrix to shader
			SendModelViewProjectionToShader(modelViewProjection);

			Vector3 lightPosDir;
			if(light.GetType() == LightType::Directional)
			{
				lightPosDir.x = modelLocalLightDir.x;
				lightPosDir.y = modelLocalLightDir.y;
				lightPosDir.z = modelLocalLightDir.z;
			}
			else
			{
				lightPosDir.x = modelLocalLightPos.x;
				lightPosDir.y = modelLocalLightPos.y;
				lightPosDir.z = modelLocalLightPos.z;
			}

			// calculate shadow volume geometry
			subRenderer->BuildShadowVolume(lightPosDir, light.GetType() == LightType::Directional);

			// send shadow volume uniforms to shader
			shadowVolumeMaterial->SendLightToShader(&light, &modelLocalLightPos, &modelLocalLightDir);

			// render the shadow volume
			subRenderer->RenderShadowVolume();
		}
	}
}

/*
 * Build the model-view-projection matrix that is used when rendering shadow volumes.
 * It is a special matrix that 'narrows' the base shadow volume to avoid Z-fighting artifacts.
 *
 * For a given mesh & light, this method gets a vector from the mesh's center [meshCenter] to the light's
 * position [modelLocalLightPos], and uses that the vector to form a rotation matrix to align that vector
 * with the Z-axis.
 *
 * Multiplying the mesh geometry by this rotation matrix and then scaling X & Y ever so slightly has the
 * effect of 'narrowing' the shadow volume around the mesh-to-light vector. This mitigates artifacts where
 * the shadow volume's sides are very close to and parallel to mesh polygons and Z-fighting occurs.
 *
 * [light] - The light for which the shadow volume is being created.
 * [meshCenter] - The center of the mesh that is casting the shadow.
 * [modelTransform] - The transform from model space to world space.
 * [modelLocalLightPos] - The position of [light] in the mesh's local space.
 * [modelLocalLightDir] - The direction of [light] in the mesh's local space.
 * [camera] - The camera for which the scene is being rendered.
 * [viewTransformInverse] - The inverse of the view transform.
 * [outTransform] - The output model-view-projection Transform.
 */
void RenderManager::BuildShadowVolumeMVPTransform(const Light& light, const Point3& meshCenter, const Transform& modelTransform, const Point3& modelLocalLightPos,
												 const Vector3& modelLocalLightDir, const Camera& camera, const Transform& viewTransformInverse, Transform& outTransform)
{
	Transform modelView;
	Transform model;

	// copy the mesh's local-to-world transform into [model]
	model.SetTo(modelTransform);

	Transform shadowVolumeViewTransform;
	Vector3 lightToMesh;

	// calculate the vector from the mesh's center to the light's position
	// and convert to world space
	if(light.GetType() == LightType::Directional)
	{
		// if light is directional, the mesh-to-light vector will
		// simply be the inverse of the light's direction
		lightToMesh = light.GetDirection();
	}
	else
	{
		Point3::Subtract(meshCenter, modelLocalLightPos, lightToMesh);
		model.TransformVector(lightToMesh);
	}
	lightToMesh.Normalize();
	// make vector go from mesh to the light
	lightToMesh.Invert();

	// the axis we want to align with (Z-axis)
	Vector3 defaultLightDir(0,0,1);

	// get the rotation quaternion from the default direction to the vector that
	// goes from the mesh's center to the light's position
	Quaternion rot = Quaternion::getRotation(defaultLightDir, lightToMesh);
	Matrix4x4 rotMatrix = rot.rotationMatrix();
	Matrix4x4 rotMatrixInverse = rotMatrix;
	rotMatrixInverse.Invert();
	shadowVolumeViewTransform.TransformBy(rotMatrix);
	shadowVolumeViewTransform.Scale(.99,.99,1.00, true);
	shadowVolumeViewTransform.TransformBy(rotMatrixInverse);

	// form MVP transform
	modelView.SetTo(shadowVolumeViewTransform);
	modelView.PreTransformBy(model);
	modelView.PreTransformBy(viewTransformInverse);
	outTransform.SetTo(modelView);
	outTransform.PreTransformBy(camera.GetProjectionTransform());
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
void RenderManager::SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& projection,  const Transform& modelViewProjection)
{
	MaterialRef activeMaterial = graphics->GetActiveMaterial();
	ASSERT_RTRN(activeMaterial.IsValid(),"RenderManager::SendTransformUniformsToShader -> activeMaterial is NULL.");

	ShaderRef shader = activeMaterial->GetShader();
	ASSERT_RTRN(shader.IsValid(),"RenderManager::SendTransformUniformsToShader -> material contains NULL shader.");

	activeMaterial->SendModelMatrixToShader(&model.matrix);
	activeMaterial->SendModelViewMatrixToShader(&modelView.matrix);
	activeMaterial->SendProjectionMatrixToShader(&projection.matrix);
	activeMaterial->SendMVPMatrixToShader(&modelViewProjection.matrix);
}

/*
 * Send only the full model-view-projection matrix stored in [modelViewProjection] to the active shader.
 * The binding information stored in the active material holds the shader variable location for this matrix;
 */
void RenderManager::SendModelViewProjectionToShader(const Transform& modelViewProjection)
{
	MaterialRef activeMaterial = graphics->GetActiveMaterial();
	ASSERT_RTRN(activeMaterial.IsValid(),"RenderManager::SendModelViewProjectionToShader -> activeMaterial is NULL.");

	ShaderRef shader = activeMaterial->GetShader();
	ASSERT_RTRN(shader.IsValid(),"RenderManager::SendModelViewProjectionToShader -> material contains NULL shader.");

	activeMaterial->SendMVPMatrixToShader(&modelViewProjection.matrix);
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

