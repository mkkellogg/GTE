#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <vector>
#include "rendermanager.h"
#include "material.h"
#include "geometry/transform.h"
#include "geometry/point/point3array.h"
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
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/submesh3D.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "filesys/filesystem.h"
#include "asset/assetimporter.h"
#include "util/datastack.h"
#include "util/time.h"
#include "global/global.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

/*
 * Single constructor, which requires pointers the Graphics and EngineObjectManager
 * singleton instances.
 */
RenderManager::RenderManager() : sceneProcessingStack(Constants::MaxObjectRecursionDepth, 1)
{
	lightCount = 0;
	ambientLightCount = 0;
	cameraCount = 0;
	sceneMeshCount = 0;
	forwardBlending = FowardBlendingMethod::Additive;
}

/*
 * Clean up
 */
RenderManager::~RenderManager()
{
	DestroyCachedShadowVolumes();
}

/*
 * Initialize. Return false if initialization false, true if it succeeds.
 */
bool RenderManager::Init()
{
	if(!sceneProcessingStack.Init())
	{
		Debug::PrintError("RenderManager::Init -> unable to initialize view transform stack.");
		return false;
	}

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	AssetImporter assetImporter;
	ShaderSource shaderSource;

	// construct shadow volume material
	assetImporter.LoadBuiltInShaderSource("shadowvolume", shaderSource);
	shadowVolumeMaterial = objectManager->CreateMaterial("ShadowVolumeMaterial", shaderSource);
	ASSERT(shadowVolumeMaterial.IsValid(), "RenderManager::Init -> Unable to create shadow volume material.", false);

	// construct SSAO material
	assetImporter.LoadBuiltInShaderSource("ssao", shaderSource);
	ssaoMaterial = objectManager->CreateMaterial("SSAO", shaderSource);
	ssaoMaterial->SetSelfLit(true);
	ASSERT(ssaoMaterial.IsValid(), "RenderManager::Init -> Unable to create SSAO material.", false);

	// construct SSAO outline material
	assetImporter.LoadBuiltInShaderSource("ssaooutline", shaderSource);
	ssaoOutlineMaterial = objectManager->CreateMaterial("SSAOOutline", shaderSource);
	ssaoOutlineMaterial->SetSelfLit(true);
	ASSERT(ssaoOutlineMaterial.IsValid(), "RenderManager::Init -> Unable to create SSAO outline material.", false);

	// construct depth-only material
	assetImporter.LoadBuiltInShaderSource("depthonly", shaderSource);
	depthOnlyMaterial = objectManager->CreateMaterial("DepthOnlyMaterial", shaderSource);
	depthOnlyMaterial->SetSelfLit(true);
	ASSERT(depthOnlyMaterial.IsValid(), "RenderManager::Init -> Unable to create depth only material.", false);

	// construct depth-value material
	assetImporter.LoadBuiltInShaderSource("depthvalue", shaderSource);
	depthValueMaterial = objectManager->CreateMaterial("DepthValueMaterial", shaderSource);
	depthValueMaterial->SetSelfLit(true);
	ASSERT(depthValueMaterial.IsValid(), "RenderManager::Init -> Unable to create depth value material.", false);

	// build depth texture off-screen render target
	const GraphicsAttributes& graphicsAttributes = Engine::Instance()->GetGraphicsEngine()->GetAttributes();
	TextureAttributes colorTextureAttributes;
	colorTextureAttributes.Format = TextureFormat::R32;
	colorTextureAttributes.FilterMode = TextureFilter::Point;
	colorTextureAttributes.WrapMode = TextureWrap::Clamp;
	depthRenderTarget = objectManager->CreateRenderTarget(true, true, colorTextureAttributes, graphicsAttributes.WindowWidth,graphicsAttributes.WindowHeight);
	ASSERT(depthRenderTarget.IsValid(), "RenderManager::Init -> Unable to create off-screen rendering surface.", false);

	TextureRef depthTexture = depthRenderTarget->GetDepthTexture();
	TextureRef colorTexture = depthRenderTarget->GetColorTexture();

	ASSERT(colorTexture.IsValid(), "RenderManager::Init -> Unable to create off-screen color buffer.", false);
	ASSERT(depthTexture.IsValid(), "RenderManager::Init -> Unable to create off-screen depth buffer.", false);

	return true;
}

/*
 * Look at the clear flags for a given camera and tell the graphics
 * system to clear the corresponding buffers.
 */
void RenderManager::ClearBuffersForCamera(const Camera& camera) const
{
	unsigned int clearBufferMask = camera.GetClearBufferMask();
	Engine::Instance()->GetGraphicsEngine()->ClearRenderBuffers(clearBufferMask);
}

/*
 * Push a new render target on to the render target stack and activate it.
 */
void RenderManager::PushRenderTarget(RenderTargetRef renderTarget)
{
	renderTargetStack.push(renderTarget);
	// activate the new render target
	Engine::Instance()->GetGraphicsEngine()->ActivateRenderTarget(renderTarget);
}

/*
 * Pop the current render target off the stack and activate the one below it.
 * If there is none below, activate the default render target.
 */
RenderTargetRef RenderManager::PopRenderTarget()
{
	RenderTargetRef old = RenderTargetRef::Null();
	if(renderTargetStack.size() > 0)
	{
		old = renderTargetStack.top();
		renderTargetStack.pop();
	}

	if(renderTargetStack.size() > 0)
	{
		RenderTargetRef top = renderTargetStack.top();
		// activate the new render target
		Engine::Instance()->GetGraphicsEngine()->ActivateRenderTarget(top);
	}
	else
	{
		// activate the default render target
		Engine::Instance()->GetGraphicsEngine()->RestoreDefaultRenderTarget();
	}

	return old;

}

/*
 * Clear [renderTargetStack].
 */
void RenderManager::ClearRenderTargetStack()
{
	renderTargetStack.empty();
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
void RenderManager::RenderScene()
{
	// render the scene from the perspective of each camera found in ProcessScene()
	for(unsigned int i=0; i < cameraCount; i ++)
	{
		RenderSceneForCamera(i);
	}
}

/*
 * Clear any caches that are set up for the render manager.
 */
void RenderManager::ClearCaches()
{
	DestroyCachedShadowVolumes();
}

/*
 * Kick off the scene processing from the root of the scene.
 */
void RenderManager::PreProcessScene()
{
	lightCount = 0;
	ambientLightCount = 0;
	cameraCount = 0;
	sceneMeshCount = 0;

	Transform cameraModelView;

	SceneObjectRef sceneRoot = (SceneObjectRef)Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
	ASSERT_RTRN(sceneRoot.IsValid(),"RenderManager::ProcessScene -> sceneRoot is NULL.");

	// gather information about the cameras, lights, and renderable meshes in the scene
	PreProcessScene(sceneRoot.GetRef(), cameraModelView);

	BuildSceneShadowVolumes();
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
 *   1. Scene objects that contain instances of Camera -> saved to [sceneCameras]
 *   2. Scene objects that contain instances of Light -> saved to [sceneAmbientLights] or [sceneLights]
 *   3. Scene objects that contain instances of Mesh3DRenderer or SkinnedMesh3DRenderer -> saved to [sceneMeshObjects]
 *
 * Later, the entire scene is rendered from the perspective of each camera in that list via RenderSceneFromCamera().
 *
 * For each Mesh3DRender and SkinnedMesh3DRenderer that is visited, the method loops through each sub-renderer
 * and calls the SubMesh3DRenderer::PreRender() Method. For skinned meshes, this method will (indirectly) perform the
 * vertex skinning transformation.
 */
void RenderManager::PreProcessScene(SceneObject& parent, Transform& aggregateTransform)
{
	Transform model;
	Transform modelInverse;

	// enforce max recursion depth
	if(RenderDepth(sceneProcessingStack) >= Constants::MaxObjectRecursionDepth - 1)return;

	for(unsigned int i = 0; i < parent.GetChildrenCount(); i++)
	{
		SceneObjectRef child = parent.GetChildAt(i);

		if(!child.IsValid())
		{
			Debug::PrintWarning("RenderManager::ProcessScene -> NULL scene object encountered.");
			continue;
		}

		// only process active scene objects
		if(child->IsActive())
		{
			// save the existing view transform
			PushTransformData(aggregateTransform, sceneProcessingStack);

			// concatenate the current view transform with that of the current scene object
			Transform& localTransform = child->GetTransform();
			aggregateTransform.TransformBy(localTransform);

			CameraRef camera = child->GetCamera();
			if(camera.IsValid() && cameraCount < MAX_CAMERAS)
			{
				// add a scene camera from which to render the scene
				// always make sure to respect the render order index of the camera,
				// the array is sort in ascending order
				for(unsigned int i = 0; i <= cameraCount; i++)
				{
					if(i == cameraCount || sceneCameras[i]->GetCamera()->GetRendeOrderIndex() > camera->GetRendeOrderIndex())
					{
						sceneCameras[cameraCount] = child;
						cameraCount++;
						break;
					}
				}
			}

			LightRef light = child->GetLight();
			if(light.IsValid())
			{
				if(light->GetType() == LightType::Ambient)
				{
					if(ambientLightCount < MAX_LIGHTS)
					{
						// add ambient light
						sceneAmbientLights[ambientLightCount] = child;
						ambientLightCount++;
					}
				}
				else
				{
					if(lightCount < MAX_LIGHTS)
					{
						// add non-ambient light
						sceneLights[lightCount] = child;
						lightCount++;
					}
				}
			}

			Mesh3DRendererRef meshRenderer = child->GetMesh3DRenderer();
			Mesh3DFilterRef meshFilter = child->GetMesh3DFilter();
			SkinnedMesh3DRendererRef skinnedMeshRenderer = child->GetSkinnedMesh3DRenderer();

			if(meshFilter.IsValid() && child->GetMesh3D().IsValid() && sceneMeshCount < MAX_SCENE_MESHES)
			{
				// check for standard mesh renderer
				if(meshRenderer.IsValid())
				{
					sceneMeshObjects[sceneMeshCount] = child;

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

				// check for skinned mesh renderer
				if(skinnedMeshRenderer.IsValid())
				{
					sceneMeshObjects[sceneMeshCount] = child;

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
			}

			// save the aggregate/global/world transform
			child->SetAggregateTransform(aggregateTransform);

			// continue recursion through child object
			PreProcessScene(child.GetRef(), aggregateTransform);

			// restore previous view transform
			PopTransformData(aggregateTransform, sceneProcessingStack);
		}
	}
}

/*
 * Render the entire scene from the perspective of a single camera. Uses [cameraIndex]
 * as an index into the array of cameras [sceneCameras] that has been found by processing the scene.
 */
void RenderManager::RenderSceneForCamera(unsigned int cameraIndex)
{
	ASSERT_RTRN(cameraIndex < cameraCount,"RenderManager::RenderSceneFromCamera -> cameraIndex out of bounds");

	SceneObjectRef objectRef= sceneCameras[cameraIndex];
	ASSERT_RTRN(objectRef.IsValid(),"RenderManager::RenderSceneFromCamera -> Camera's scene object is not valid.");

	CameraRef cameraRef = objectRef->GetCamera();
	ASSERT_RTRN(cameraRef.IsValid(),"RenderManager::RenderSceneFromCamera -> Camera is not valid.");
	Camera& camera = cameraRef.GetRef();

	SceneObjectRef sceneRoot = (SceneObjectRef)Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
	ASSERT_RTRN(sceneRoot.IsValid(),"RenderManager::RenderSceneFromCamera -> sceneRoot is NULL.");

	// currently we use forward rendering
	ForwardRenderSceneForCamera(camera);
}

/*
 * Render all the meshes in the scene using forward rendering. The camera's inverted transform is used
 * as the view transform. The reason the inverse is used is because on the GPU side of things the view
 * transform is used to move the world relative to the camera, rather than move the camera in the world.
 *
 * Since this method uses a forward-rendering approach, each mesh is rendered once for each light and the output from
 * each pass is combined with the others using additive blending. This method calls ForwardRenderSceneForLight(),
 * which then calls ForwardRenderSceneObject(). The additive blending occurs in the ForwardRenderSceneObject() method.
 */
void RenderManager::ForwardRenderSceneForCamera(Camera& camera)
{
	SceneObjectRef cameraObject = camera.GetSceneObject();
	ASSERT_RTRN(cameraObject.IsValid(),"RenderManager::ForwardRenderSceneForCamera -> Camera is not attached to a scene object.");

	// render the scene using the view transform of the current camera
	const Transform& cameraTransform = camera.GetSceneObject()->GetAggregateTransform();

	// we invert the camera's transform because at the shader level, the view transform is
	// really moving the world relative to the camera, rather than moving the camera
	// in the world
	Transform viewInverse;
	viewInverse.SetTo(cameraTransform);
	viewInverse.Invert();

	// clear the list of objects that have been rendered at least once. this list is used to
	// determine if blending should be turned on or off. if an object is being rendered for the
	// first time, additive blending should be off; otherwise it should be on.
	renderedObjects.clear();

	// get the camera's render target as the active render target
	ClearRenderTargetStack();
	PushRenderTarget(camera.GetRenderTarget());

	// clear the appropriate render buffers for this camera
	ClearBuffersForCamera(camera);

	// we have not yet rendered any ambient lights
	bool renderedAmbient = false;

	// loop through each ambient light and render the scene for that light
	for(unsigned int l = 0; l < ambientLightCount; l++)
	{
		SceneObjectRef lightObject = sceneAmbientLights[l];
		ASSERT_RTRN(lightObject.IsValid(), "RenderManager::ForwardRenderScene -> Ambient light's scene object is not valid.");

		LightRef lightRef = lightObject->GetLight();
		ASSERT_RTRN(lightRef.IsValid(), "RenderManager::ForwardRenderScene -> Ambient light is not valid.");

		// render all objects in the scene that have non self-lit materials
		ForwardRenderSceneForLight(lightRef.GetRef(), lightObject->GetAggregateTransform(), viewInverse, camera);
		renderedAmbient = true;
	}

	// if no ambient lights were rendered, then we need to fill the depth buffer with the scene to allow
	// for proper shadow volume rendering and depth-buffer culling
	ForwardRenderDepthBuffer(viewInverse, camera);

	// perform the standard screen-space ambient occlusion pass
	if(renderedAmbient && camera.IsSSAOEnabled() && camera.GetSSAORenderMode() == SSAORenderMode::Standard)ForwardRenderSceneSSAO(viewInverse, camera);

	// loop through each regular light and render scene for that light
	for(unsigned int l = 0; l < lightCount; l++)
	{
		SceneObjectRef lightObject = sceneLights[l];
		ASSERT_RTRN(lightObject.IsValid(), "RenderManager::ForwardRenderScene -> Light's scene object is not valid.");

		LightRef lightRef = lightObject->GetLight();
		ASSERT_RTRN(lightRef.IsValid(), "RenderManager::ForwardRenderScene -> Light is not valid.");

		// render all objects in the scene that have non self-lit materials
		ForwardRenderSceneForLight(lightRef.GetRef(), lightObject->GetAggregateTransform(), viewInverse, camera);
	}

	// perform the screen-space ambient occlusion pass as an outline effect
	if(camera.IsSSAOEnabled() && camera.GetSSAORenderMode() == SSAORenderMode::Outline)ForwardRenderSceneSSAO(viewInverse, camera);

	// render all self-lit objects in the scene once
	ForwardRenderSceneForSelfLitMaterials(viewInverse, camera);

	// if this camera has a skybox that is set up and enabled, then we want to render it
	if(camera.IsSkyboxSetup() && camera.IsSkyboxEnabled())
	{
		ForwardRenderSkyboxForCamera(camera, viewInverse);
	}

	PopRenderTarget();
}

/*
 * Forward-Render the scene for a single light [light] from the perspective
 * of [camera] using [viewTransformInverse] as the camera's position and orientation.
 *
 * This method performs two passes:
 *
 * Pass 1: If [light] is not ambient, render shadow volumes for all meshes in the scene for [light] into the stencil buffer.
 * Pass 2: Perform actual rendering of all meshes in the scene for [light]. If [light] is not ambient, this pass will
 *         exclude screen pixels that are hidden from [light] based on the stencil buffer contents from pass 0. Is [light]
 *         is ambient, then this pass will perform a standard render of all meshes in the scene.
 */

void RenderManager::ForwardRenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& viewTransformInverse, const Camera& camera)
{
	Transform modelView;
	Transform model;
	Transform modelInverse;

	Point3 lightPosition;
	lightFullTransform.TransformPoint(lightPosition);

	RenderMode currentRenderMode = RenderMode::None;

	enum RenderPass
	{
		ShadowVolumeRender = 0,
		StandardRender = 1,
		_PassCount = 2
	};

	for(int pass = 0; pass < RenderPass::_PassCount; pass++)
	{
		if(pass == ShadowVolumeRender) // shadow volume pass
		{
			// check if this light can cast shadows; if not we skip this pass
			if(light.GetShadowsEnabled() && light.GetType() != LightType::Ambient)
			{
				currentRenderMode = RenderMode::ShadowVolumeRender;
				Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::ShadowVolumeRender);
			}
			else
				continue;
		}

		// loop through each mesh-containing SceneObject in [sceneMeshObjects]
		for(unsigned int s = 0; s < sceneMeshCount; s++)
		{
			SceneObjectRef childRef = sceneMeshObjects[s];

			Mesh3DRef mesh = childRef->GetMesh3D();
			Mesh3DFilterRef filter = childRef->GetMesh3DFilter();
			SceneObject * child = childRef.GetPtr();

			// copy the full transform of the scene object, including those of all ancestors
			Transform full;
			SceneObjectTransform::GetWorldTransform(full, childRef, true, false);

			// make sure the current mesh should not be culled from [light].
			if(!ShouldCullFromCamera(camera, *child) &&
				!ShouldCullFromLight(light, lightPosition, full, *child))
			{
				if(pass == ShadowVolumeRender) // shadow volume pass
				{
					if(filter->GetCastShadows())
					{
						RenderShadowVolumesForSceneObject(*child, light, lightPosition, viewTransformInverse, camera);
					}
				}
				else if(pass == StandardRender) // normal rendering pass
				{
					// check if this light can cast shadows and the mesh can receive shadows, if not do standard (shadow-less) rendering
					if(light.GetShadowsEnabled() && light.GetType() != LightType::Ambient && filter->GetReceiveShadows())
					{
						if(currentRenderMode != RenderMode::StandardWithShadowVolumeTest)
						{
							currentRenderMode = RenderMode::StandardWithShadowVolumeTest;
							Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::StandardWithShadowVolumeTest);
						}
					}
					else if(currentRenderMode != RenderMode::Standard)
					{
						currentRenderMode = RenderMode::Standard;
						Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::Standard);
					}

					// set up lighting descriptor for non self-lit lighting
					LightingDescriptor lightingDescriptor(&light, &lightPosition, false);
					ForwardRenderSceneObject(*child, lightingDescriptor, viewTransformInverse, camera, MaterialRef::Null(), true, true, FowardBlendingFilter::OnlyIfRendered);
				}
			}
		}
	}
}

/*
 * Forward-Render all the meshes found in ProcessScene() that have self-lit materials from the perspective
 * of [camera] using [viewTransformInverse] as the camera's position and orientation.
 */
void RenderManager::ForwardRenderSceneForSelfLitMaterials(const Transform& viewTransformInverse , const Camera& camera)
{
	ForwardRenderSceneWithSelfLitLighting(viewTransformInverse, camera, MaterialRef::Null(), true, true, FowardBlendingFilter::OnlyIfRendered);
}

/*
 * Render the skybox for [camera] using [viewTransformInverse] as the view transformation.
 * [viewTransformation] should be the inverse of the camera's transformation.
 */
void RenderManager::ForwardRenderSkyboxForCamera(Camera& camera, const Transform& viewTransformInverse)
{
	Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::Standard);

	// ensure the camera has a valid skybox
	if(camera.IsSkyboxSetup() && camera.IsSkyboxEnabled())
	{
		SceneObjectRef cameraObject = camera.GetSceneObject();
		ASSERT_RTRN(cameraObject.IsValid(),"RenderManager::RenderSkyboxForCamera -> Camera is not attached to a scene object.");

		// retrieve the scene objects for the camera and for the camera's skybox
		SceneObjectRef skyboxObject = camera.GetSkyboxSceneObject();
		ASSERT_RTRN(skyboxObject.IsValid(),"RenderManager::RenderSkyboxForCamera -> Camera has invalid skybox scene object.");

		/// get the world space location of the camera
		Point3 cameraOrigin;
		cameraObject->GetAggregateTransform().TransformPoint(cameraOrigin);

		// get world space location of the skybox
		Point3 skyboxOrigin;
		skyboxObject->GetAggregateTransform().TransformPoint(skyboxOrigin);

		// calculate the distance between the camera and the center of the skybox
		Vector3 trans;
		Point3::Subtract(cameraOrigin, skyboxOrigin, trans);

		// update the skybox's position to be equal to to the camera's position, but leave
		// the skybox's orientation as default. if we matched the skybox's orientation to
		// the camera's orientation, we'd always see the exact same area of the skybox, no
		// matter where the camera was facing.
		Transform base;
		base.SetTo(skyboxObject->GetAggregateTransform());
		base.Translate(trans, false);
		skyboxObject->SetAggregateTransform(base);
		skyboxObject->GetTransform().Translate(trans, false);

		// render the skybox
		skyboxObject->SetActive(true);
		LightingDescriptor lightingDescriptor(NULL, NULL, true);
		ForwardRenderSceneObject(skyboxObject.GetRef(), lightingDescriptor, viewTransformInverse, camera, MaterialRef::Null(), true, true, FowardBlendingFilter::Never);
		skyboxObject->SetActive(false);
	}
}

/*
 * Render the scene to only the depth-buffer. Render from the perspective
 * of [camera] using [viewTransformInverse] as the camera's position and orientation.
 */
void RenderManager::ForwardRenderDepthBuffer(const Transform& viewTransformInverse, const Camera& camera)
{
	ForwardRenderSceneWithSelfLitLighting(viewTransformInverse, camera, depthOnlyMaterial, false, false, FowardBlendingFilter::Never);
}

/*
 * Render the screen-space ambient occlusion for the scene. Render from the perspective
 * of [camera] using [viewTransformInverse] as the camera's position and orientation.
 */
void RenderManager::ForwardRenderSceneSSAO(const Transform& viewTransformInverse, const Camera& camera)
{
	Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::Standard);
	GraphicsAttributes attributes = Engine::Instance()->GetGraphicsEngine()->GetAttributes();

	// activate the off-screen render target
	PushRenderTarget(depthRenderTarget);

	// clear the relevant buffers in the off-screen render target
	IntMask clearMask = IntMaskUtil::CreateIntMask();
	if(depthRenderTarget->HasBuffer(RenderBufferType::Color))IntMaskUtil::SetBitForMask(&clearMask, (unsigned int)RenderBufferType::Color);
	if(depthRenderTarget->HasBuffer(RenderBufferType::Depth))IntMaskUtil::SetBitForMask(&clearMask, (unsigned int)RenderBufferType::Depth);
	Engine::Instance()->GetGraphicsEngine()->ClearRenderBuffers(clearMask);

	// render the depth values for the scene to the off-screen color texture
	ForwardRenderSceneWithSelfLitLighting(viewTransformInverse, camera, depthValueMaterial, false, true, FowardBlendingFilter::Never);

	// restore previous render target
	PopRenderTarget();

	Matrix4x4 projectionInvMat;
	camera.GetProjectionTransform().CopyMatrix(projectionInvMat);
	projectionInvMat.Invert();

	// retrieve the color texture (which contains depth values) from the off-screen render target
	TextureRef depthTexture = depthRenderTarget->GetColorTexture();

	// set SSAO material values
	ssaoOutlineMaterial->SetTexture(depthTexture, "DEPTH_TEXTURE");
	ssaoOutlineMaterial->SetMatrix4x4(projectionInvMat, "INV_PROJECTION_MATRIX");
	ssaoOutlineMaterial->SetUniform1f(.5, "DISTANCE_THRESHHOLD");
	ssaoOutlineMaterial->SetUniform2f(.3,.75, "FILTER_RADIUS");
	ssaoOutlineMaterial->SetUniform1f(attributes.WindowWidth, "SCREEN_WIDTH");
	ssaoOutlineMaterial->SetUniform1f(attributes.WindowHeight, "SCREEN_HEIGHT");

	FowardBlendingMethod currentFoward = GetForwardBlending();

	// set forward rendering blending to subtractive since we need to darken areas of the
	// scene that are occluded.
	SetForwardBlending(FowardBlendingMethod::Subtractive);
	// rendering scene with SSAO material and filter out non-static objects
	ForwardRenderSceneWithSelfLitLighting(viewTransformInverse, camera, ssaoOutlineMaterial, false, true, FowardBlendingFilter::Always, [=](SceneObjectRef sceneObject)
	{
		return !sceneObject->IsStatic();
	});
	// restore previous forward rendering blend mode
	SetForwardBlending(currentFoward);
}

/*
 * Same as main ForwardRenderSceneWithSelfLitLighting() method, except it passes nullptr
 * as the value for [filterFunction].
 */
void RenderManager::ForwardRenderSceneWithSelfLitLighting(const Transform& viewTransformInverse, const Camera& camera, MaterialRef material, bool flagRendered,
														  bool renderMoreThanOnce, FowardBlendingFilter blendingFilter)
{
	ForwardRenderSceneWithSelfLitLighting(viewTransformInverse, camera, material, flagRendered, renderMoreThanOnce, blendingFilter, nullptr);
}

/*
 * Render all objects in the scene with a self-lit lighting descriptor. Use [material] as the
 * override material (if it is valid, needs to be self-lit), otherwise all meshes will be rendered
 * with their own materials (if they are self-lit).
 *
 * [camera] - The Camera object for which rendering is taking place.
 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
 * [material] - If this is valid, it will be used to render all meshes.
 * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
 *                  works for that mesh on future rendering passes.
 * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
 * [blendingFilter] - Determines how forward-rendering blending will be applied.
 * [filterFunction] - Used to filter out select scene objects.
 *
 * Render from the perspective of [camera] using [viewTransformInverse] as the camera's position and orientation.
 */
void RenderManager::ForwardRenderSceneWithSelfLitLighting(const Transform& viewTransformInverse, const Camera& camera, MaterialRef material, bool flagRendered,
		   	   	   	   	   	   	   	   	   	   	   	      bool renderMoreThanOnce, FowardBlendingFilter blendingFilter,  std::function<bool(SceneObjectRef)> filterFunction)
{
	Engine::Instance()->GetGraphicsEngine()->EnterRenderMode(RenderMode::Standard);

	// loop through each mesh-containing SceneObject in [sceneMeshObjects]
	for(unsigned int s = 0; s < sceneMeshCount; s++)
	{
		SceneObjectRef childRef = sceneMeshObjects[s];

		if(ShouldCullFromCamera(camera, childRef.GetRef()))
		{
			continue;
		}

		// execute filter function (if one is specified)
		if(filterFunction != nullptr)
		{
			bool filter = filterFunction(childRef);
			if(filter)continue;
		}

		Mesh3DRef mesh = childRef->GetMesh3D();
		SceneObject * child = childRef.GetPtr();

		// copy the full transform of the scene object, including those of all ancestors
		Transform full;
		SceneObjectTransform::GetWorldTransform(full, childRef, true, false);

		LightingDescriptor lightingDescriptor(NULL, NULL, true);
		ForwardRenderSceneObject(*child, lightingDescriptor, viewTransformInverse, camera, material, flagRendered, renderMoreThanOnce, blendingFilter);
	}
}
/*
 * Forward-Render the meshes attached to [sceneObject].
 *
 * [lightingDescriptor] - Describes the lighting to be used for rendering (if there is any).
 * [camera] - The Camera object for which rendering is taking place.
 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
 * [materialOverride] - If this is valid, it will be used to render all meshes.
 * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
 *                  works for that mesh on future rendering passes.
 * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
 * [blendingFilter] - Determines how forward-rendering blending will be applied.
 */
void RenderManager::ForwardRenderSceneObject(SceneObject& sceneObject, const LightingDescriptor& lightingDescriptor, const Transform& viewTransformInverse,
											 const Camera& camera, MaterialRef materialOverride, bool flagRendered, bool renderMoreThanOnce, FowardBlendingFilter blendingFilter)
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
		Mesh3DRef mesh = renderer->GetTargetMesh();

		ASSERT_RTRN(mesh.IsValid(),"RenderManager::RenderSceneObjectMeshes -> renderer returned NULL mesh.");
		ASSERT_RTRN(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(),"RenderManager::RenderSceneObjectMeshes -> Sub mesh count does not match sub renderer count!.");
		ASSERT_RTRN(renderer->GetMaterialCount() > 0,"RenderManager::RenderSceneObjectMeshes -> renderer has no materials.");

		unsigned int materialIndex = 0;
		bool doMaterialOvverride = materialOverride.IsValid() ? true : false;

		model.SetTo(sceneObject.GetAggregateTransform());

		// concatenate model transform with inverted view transform, and then with
		// the camera's projection transform.
		modelView.SetTo(model);
		modelView.PreTransformBy(viewTransformInverse);
		modelViewProjection.SetTo(modelView);
		modelViewProjection.PreTransformBy(camera.GetProjectionTransform());

		// loop through each sub-renderer and render its mesh(es)
		for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
		{
			MaterialRef currentMaterial;

			// if we have an override material, we use that for every mesh
			if(doMaterialOvverride)
				currentMaterial = materialOverride;
			else
				currentMaterial = renderer->GetMaterial(materialIndex);

			SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
			SubMesh3DRef subMesh = mesh->GetSubMesh(i);

			ASSERT_RTRN(currentMaterial.IsValid(),"RenderManager::RenderSceneObjectMeshes -> NULL material encountered.")
			ASSERT_RTRN(subRenderer.IsValid(), "RenderManager::RenderSceneObjectMeshes -> NULL sub renderer encountered.");
			ASSERT_RTRN(subMesh.IsValid(), "RenderManager::RenderSceneObjectMeshes -> NULL sub mesh encountered.");

			// determine if this mesh has been rendered before
			ObjectPairKey key(sceneObject.GetObjectID(), subMesh->GetObjectID());
			bool rendered = renderedObjects[key];

			bool skipMesh = false;
			// current material is self-lit, the we only want to render if the
			//lighting descriptor specifies self-lit and vice-versa
			if(currentMaterial->IsSelfLit() && !lightingDescriptor.SelfLit)skipMesh = true;
			if(!currentMaterial->IsSelfLit() && lightingDescriptor.SelfLit)skipMesh = true;
			if(rendered && !renderMoreThanOnce)skipMesh = true;

			if(!skipMesh)
			{
				// activate the material, which will switch the GPU's active shader to
				// the one associated with the material
				ActivateMaterial(currentMaterial);
				// send uniforms set for the new material to its shader
				SendActiveMaterialUniformsToShader();

				// send light data to the active shader (if not self-lit)
				if(!lightingDescriptor.SelfLit)
				{
					currentMaterial->SendLightToShader(lightingDescriptor.LightObject, lightingDescriptor.LightPosition, NULL);
				}
				// pass concatenated modelViewTransform and projection transforms to shader
				SendTransformUniformsToShader(model, modelView, camera.GetProjectionTransform(), modelViewProjection);

				// if this sub mesh has already been rendered by this camera, then we want to use
				// additive blending to combine it with the output from other lights. Otherwise
				// turn off blending and render.
				if((rendered && blendingFilter==FowardBlendingFilter::OnlyIfRendered) || (blendingFilter==FowardBlendingFilter::Always))
				{
					if(GetForwardBlending() == FowardBlendingMethod::Subtractive)
					{
						Engine::Instance()->GetGraphicsEngine()->SetBlendingEnabled(true);
						Engine::Instance()->GetGraphicsEngine()->SetBlendingFunction(BlendingProperty::Zero,BlendingProperty::SrcAlpha);
					}
					else
					{
						Engine::Instance()->GetGraphicsEngine()->SetBlendingEnabled(true);
						Engine::Instance()->GetGraphicsEngine()->SetBlendingFunction(BlendingProperty::One,BlendingProperty::One);
					}
				}
				else
				{
					Engine::Instance()->GetGraphicsEngine()->SetBlendingEnabled(false);
				}

				// render the current mesh
				subRenderer->Render();

				// flag the current scene object as being rendered (at least once)
				if(flagRendered)renderedObjects[key] = true;
			}

			// Advance material index. Renderer can have any number of materials > 0; it does not have to match
			// the number of sub meshes. If the end of the material array is reached, loop back to the beginning.
			if(!doMaterialOvverride)
			{
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
 * Render the shadow volumes for the meshes attached to [sceneObject] for [light]. This essentially means altering the
 * stencil buffer to reflect areas of the rendered scene that are shadowed from [light] by the meshes attached to [sceneObject].
 *
 * [lightPosition] - The world space position of [light].
 * [camera] - The Camera object for which rendering is taking place.
 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
 */
void RenderManager::RenderShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition,
													  const Transform& viewTransformInverse, const Camera& camera)
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
		Mesh3DRef mesh = renderer->GetTargetMesh();

		ASSERT_RTRN(mesh.IsValid(),"RenderManager::RenderShadowVolumesForSceneObject -> renderer returned NULL mesh.");
		ASSERT_RTRN(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(),"RenderManager::RenderShadowVolumesForSceneObject -> Sub mesh count does not match sub renderer count!.");
		ASSERT_RTRN(renderer->GetMaterialCount() > 0,"RenderManager::RenderShadowVolumesForSceneObject -> renderer has no materials.");

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

		// loop through each sub-renderer and render the shadow volume for its sub-mesh
		for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
		{
			SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
			SubMesh3DRef subMesh = mesh->GetSubMesh(i);

			ASSERT_RTRN(subRenderer.IsValid(), "RenderManager::RenderShadowVolumesForSceneObject -> NULL sub renderer encountered.");
			ASSERT_RTRN(subMesh.IsValid(), "RenderManager::RenderShadowVolumesForSceneObject -> NULL sub mesh encountered.");

			// build special MVP transform for rendering shadow volumes
			float scaleFactor = subRenderer->GetUseBackSetShadowVolume() ? .99 : 1;
			BuildShadowVolumeMVPTransform(light, subMesh->GetCenter(), model, modelLocalLightPos, modelLocalLightDir, camera, viewTransformInverse, modelViewProjection, scaleFactor, scaleFactor);

			// activate the material, which will switch the GPU's active shader to
			// the one associated with the material
			ActivateMaterial(shadowVolumeMaterial);

			// set the epsilon offset for the shadow volume shader
			if(subRenderer->GetUseBackSetShadowVolume())shadowVolumeMaterial->SetUniform1f(.0002, "EPSILON");
			else shadowVolumeMaterial->SetUniform1f(.2, "EPSILON");

			// send uniforms set for the shadow volume material to its shader
			SendActiveMaterialUniformsToShader();
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

			// send shadow volume uniforms to shader
			shadowVolumeMaterial->SendLightToShader(&light, &modelLocalLightPos, &modelLocalLightDir);

			Light& castLight = const_cast<Light&>(light);
			SceneObjectRef lightObject = castLight.GetSceneObject();

			ObjectPairKey cacheKey;

			// form cache key from sub-renderer's object ID and light's object ID
			cacheKey.ObjectAID = subRenderer->GetObjectID();
			cacheKey.ObjectBID = light.GetObjectID();

			const Point3Array * cachedShadowVolume = NULL;
			cachedShadowVolume = GetCachedShadowVolume(cacheKey);

			// render the shadow volume if it is valid
			if(cachedShadowVolume != NULL)
			{
				subRenderer->RenderShadowVolume(cachedShadowVolume);
			}
		}
	}
}

/*
 * Build shadow volumes for all relevant meshes for all shadow casting lights.
 */
void RenderManager::BuildSceneShadowVolumes()
{
	// loop through each light in [sceneLights]
	for(unsigned int l = 0; l < lightCount; l++)
	{
		SceneObjectRef lightObject = sceneLights[l];
		ASSERT_RTRN(lightObject.IsValid(), "RenderManager::BuildSceneShadowVolumes -> Light's scene object is not valid.");

		// verify the light is active
		if(lightObject->IsActive())
		{
			LightRef lightRef = lightObject->GetLight();
			ASSERT_RTRN(lightRef.IsValid(), "RenderManager::BuildSceneShadowVolumes -> Light is not valid.");

			// verify that this light casts shadows
			if(lightRef->GetShadowsEnabled())
			{
				Transform lightFullTransform;
				lightFullTransform.SetTo(lightRef->GetSceneObject()->GetAggregateTransform());
				BuildShadowVolumesForLight(lightRef.GetRef(), lightFullTransform);
			}
		}
	}
}

/*
 * Build shadow volumes for all relevant meshes for [light]. Use [lightFullTransform] as the
 * local-to-world transformation for the light.
 */
void RenderManager::BuildShadowVolumesForLight(const Light& light, const Transform& lightFullTransform)
{
	Transform modelView;
	Transform model;
	Transform modelInverse;

	Point3 lightPosition;
	lightFullTransform.TransformPoint(lightPosition);

	// loop through each mesh-containing SceneObject in [sceneMeshObjects]
	for(unsigned int s = 0; s < sceneMeshCount; s++)
	{
		SceneObjectRef childRef = sceneMeshObjects[s];

		if(childRef->IsActive())
		{
			Mesh3DRef mesh = childRef->GetMesh3D();
			Mesh3DFilterRef filter = childRef->GetMesh3DFilter();
			SceneObject * child = childRef.GetPtr();

			// copy the full transform of the scene object, including those of all ancestors
			Transform full;
			SceneObjectTransform::GetWorldTransform(full, childRef, true, false);

			// make sure the current mesh should not be culled from [light].
			if(!ShouldCullFromLight(light, lightPosition, full, *child))
			{
				BuildShadowVolumesForSceneObject(*child, light, lightPosition);
			}
		}
	}
}

/*
 * Build (and cache) shadow volumes for the meshes attached to [sceneObject] for [light], using
 * [lightPosition] as the light's world position.
 */
void RenderManager::BuildShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition)
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
		Mesh3DRef mesh = renderer->GetTargetMesh();

		ASSERT_RTRN(mesh.IsValid(),"RenderManager::BuildShadowVolumesForSceneObject -> renderer returned NULL mesh.");
		ASSERT_RTRN(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(),"RenderManager::BuildShadowVolumesForSceneObject -> Sub mesh count does not match sub renderer count!.");
		ASSERT_RTRN(renderer->GetMaterialCount() > 0,"RenderManager::BuildShadowVolumesForSceneObject -> renderer has no materials.");

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

		// loop through each sub-renderer and render the shadow volume for its sub-mesh
		for(unsigned int i=0; i < renderer->GetSubRendererCount(); i++)
		{
			SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
			SubMesh3DRef subMesh = mesh->GetSubMesh(i);

			ASSERT_RTRN(subRenderer.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> NULL sub renderer encountered.");
			ASSERT_RTRN(subMesh.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> NULL sub mesh encountered.");

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

			Light& castLight = const_cast<Light&>(light);
			SceneObjectRef lightObject = castLight.GetSceneObject();

			ObjectPairKey cacheKey;
			bool cached = false;

			// a shadow volume is dynamic if the light's scene object is not static or the mesh's
			// scene object is not static
			bool dynamic = !(sceneObject.IsStatic()) || !(lightObject->IsStatic());

			// form cache key from sub-renderer's object ID and light's object ID
			cacheKey.ObjectAID = subRenderer->GetObjectID();
			cacheKey.ObjectBID = light.GetObjectID();

			const Point3Array * cachedShadowVolume = GetCachedShadowVolume(cacheKey);

			// check if this shadow volume is already cached
			if(cachedShadowVolume != NULL)
			{
				cached = true;
			}

			if(!cached || dynamic) // always rebuild dynamic shadow volumes
			{
				// calculate shadow volume geometry
				if(subRenderer->GetUseBackSetShadowVolume())subRenderer->BuildShadowVolume(lightPosDir, light.GetType() == LightType::Directional, true);
				else subRenderer->BuildShadowVolume(lightPosDir, light.GetType() == LightType::Directional, false);

				// cache shadow volume for later rendering
				CacheShadowVolume(cacheKey, subRenderer->GetShadowVolumePositions());
			}
		}
	}
}

/*
 * Validate [sceneObject] for rendering. This means making sure that there
 * is a mesh and a mesh renderer present, and verifying that [sceneObject]
 * is active.
 */
bool RenderManager::ValidateSceneObjectForRendering(SceneObjectRef sceneObject) const
{
	if(!sceneObject.IsValid())
	{
		Debug::PrintWarning("RenderManager::IsSceneObjectReadyForRendering -> NULL scene object encountered.");
		return false;
	}

	SceneObject * object = sceneObject.GetPtr();

	if(!object->IsActive())return false;

	Mesh3DRenderer * renderer = NULL;

	// check if current SceneObject has a mesh & renderer
	if(object->GetMesh3DRenderer().IsValid())renderer = object->GetMesh3DRenderer().GetPtr();
	else if(object->GetSkinnedMesh3DRenderer().IsValid())renderer = (Mesh3DRenderer *)object->GetSkinnedMesh3DRenderer().GetPtr();
	else
	{
		Debug::PrintWarning("RenderManager::IsSceneObjectReadyForRendering -> Could not find renderer for mesh.");
		return false;
	}

	Mesh3DRef mesh = renderer->GetTargetMesh();

	if(!mesh.IsValid())
	{
		Debug::PrintWarning("RenderManager::IsSceneObjectReadyForRendering -> Invalid mesh encountered.");
		return false;
	}

	return true;
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
 * [xScale] - factor by which to scale the shadow volume along the x-axis.
 * [yScale] - factor by which to scale the shadow volume along the y-axis.
 */
void RenderManager::BuildShadowVolumeMVPTransform(const Light& light, const Point3& meshCenter, const Transform& modelTransform, const Point3& modelLocalLightPos,
												 const Vector3& modelLocalLightDir, const Camera& camera, const Transform& viewTransformInverse, Transform& outTransform,
												 float xScale, float yScale) const
{
	Transform modelView;
	Transform model;
	Transform modelInverse;

	// copy the mesh's local-to-world transform into [model]
	model.SetTo(modelTransform);
	modelInverse.SetTo(model);
	modelInverse.Invert();

	Transform shadowVolumeViewTransform;
	Vector3 lightToMesh;
	Point3 origin;

	// calculate the vector from the mesh's center to the light's position
	// in model local space
	if(light.GetType() == LightType::Directional)
	{
		// if light is directional, the mesh-to-light vector will
		// simply be the inverse of the light's direction
		lightToMesh = light.GetDirection();
		modelInverse.TransformVector(lightToMesh);
	}
	else
	{
		Point3::Subtract(meshCenter, modelLocalLightPos, lightToMesh);
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
	shadowVolumeViewTransform.TransformBy(rotMatrixInverse);
	shadowVolumeViewTransform.Scale(xScale,yScale,1.00, true);
	shadowVolumeViewTransform.TransformBy(rotMatrix);

	// form MVP transform
	modelView.SetTo(shadowVolumeViewTransform);
	modelView.PreTransformBy(model);
	modelView.PreTransformBy(viewTransformInverse);
	outTransform.SetTo(modelView);
	outTransform.PreTransformBy(camera.GetProjectionTransform());
}

/*
 * Store a copy of a shadow volume in [shadowVolumeCache], keyed by [key].
 */
void RenderManager::CacheShadowVolume(const ObjectPairKey& key, const Point3Array * shadowVolume)
{
	ASSERT_RTRN(shadowVolume != NULL, "RenderManager::CacheShadowVolume -> Shadow volume is NULL.");

	bool needsInit = false;
	Point3Array * target = NULL;

	if(!HasCachedShadowVolume(key))
	{
		needsInit = true;
	}
	else
	{
		target = shadowVolumeCache[key];
		if(target->GetReservedCount() != shadowVolume->GetReservedCount())
		{
			ClearCachedShadowVolume(key);
			needsInit = true;
		}
	}

	if(needsInit)
	{
		target = new Point3Array();
		ASSERT_RTRN(target != NULL, "RenderManager::CacheShadowVolume -> Unable to allocate shadow volume copy.");

		bool initSuccess = target->Init(shadowVolume->GetReservedCount());
		ASSERT_RTRN(initSuccess, "RenderManager::CacheShadowVolume -> Unable to initialize shadow volume copy.");

		target->SetCount(shadowVolume->GetCount());
		shadowVolumeCache[key] = target;
	}

	target->SetCount(shadowVolume->GetCount());
	shadowVolume->CopyTo(target);
}

/*
 * Remove the shadow volume cached for [key] (if it exists).
 */
void RenderManager::ClearCachedShadowVolume(const ObjectPairKey& key)
{
	if(HasCachedShadowVolume(key))
	{
		Point3Array* shadowVolume =  shadowVolumeCache[key];
		shadowVolumeCache.erase(key);
		if(shadowVolume != NULL)
		{
			delete shadowVolume;
		}
	}
}

/*
 * Is a shadow volume cached for [key].
 */
bool RenderManager::HasCachedShadowVolume(const ObjectPairKey& key)  const
{
	if(shadowVolumeCache.find(key) != shadowVolumeCache.end())
	{
		return true;
	}

	return false;
}

/*
 * Get cached shadow volume for [key].
 */
const Point3Array * RenderManager::GetCachedShadowVolume(const ObjectPairKey& key)
{
	if(HasCachedShadowVolume(key))
	{
		const Point3Array * shadowVolume = shadowVolumeCache[key];
		return shadowVolume;
	}

	return NULL;
}

/*
 * Remove and delete all cached shadow volumes.
 */
void RenderManager::DestroyCachedShadowVolumes()
{
	for (unsigned i = 0; i < shadowVolumeCache.bucket_count(); ++i)
	{
		for (auto iter = shadowVolumeCache.begin(i); iter!= shadowVolumeCache.end(i); ++iter )
	    {
			Point3Array * shadowVolume = iter->second;
			delete shadowVolume;
	    }
	}
	shadowVolumeCache.clear();
}

/*
 * Set the blending method to be used in forward rendering.
 */
void RenderManager::SetForwardBlending(FowardBlendingMethod method)
{
	forwardBlending = method;
}

/*
 * Get the blending method to be used in forward rendering.
 */
FowardBlendingMethod RenderManager::GetForwardBlending() const
{
	return forwardBlending;
}

/*
 * Should [camera] render the meshes on [sceneObject]? This is determined by comparing the
 * layer mask of [sceneObject] with the culling mask of [camera].
 */
bool RenderManager::ShouldCullFromCamera(const Camera& camera, const SceneObject& sceneObject) const
{
	// make sure camera's culling mask includes at least one of layers of [sceneObject]
	return !(Engine::Instance()->GetEngineObjectManager()->GetLayerManager().AtLeastOneLayerInCommon(sceneObject.GetLayerMask(), camera.GetCullingMask()));
}

/*
 * Check if [mesh] should be rendered with [light], first based on the culling mask of the light and the layer to
 * which [sceneObject] belongs, and then based on the distance of the center of [mesh] from [lightPosition].
 */
bool RenderManager::ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform,  const SceneObject& sceneObject) const
{
	// exclude objects that have layer masks that are not compatible
	// with the culling mask of the light contained in [lightingDescriptor].
	IntMask layerMask = sceneObject.GetLayerMask();
	IntMask cullingMask = light.GetCullingMask();
	if(!Engine::Instance()->GetEngineObjectManager()->GetLayerManager().AtLeastOneLayerInCommon(layerMask, cullingMask))
	{
		return true;
	}

	if(light.GetType() == LightType::Directional || light.GetType() == LightType::Ambient )
	{
		return false;
	}

	SceneObject& sceneObj = const_cast<SceneObject&>(sceneObject);
	Mesh3DRef meshRef = sceneObj.GetMesh3D();

	if(meshRef.IsValid())
	{
		Mesh3D& mesh = meshRef.GetRef();

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
	MaterialRef activeMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();
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
	MaterialRef activeMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();
	ASSERT_RTRN(activeMaterial.IsValid(),"RenderManager::SendModelViewProjectionToShader -> activeMaterial is NULL.");

	ShaderRef shader = activeMaterial->GetShader();
	ASSERT_RTRN(shader.IsValid(),"RenderManager::SendModelViewProjectionToShader -> material contains NULL shader.");

	activeMaterial->SendMVPMatrixToShader(&modelViewProjection.matrix);
}

/*
 * Send any custom uniforms specified by the active material to the active shader
 */
void RenderManager::SendActiveMaterialUniformsToShader()  const
{
	MaterialRef activeMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();
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
	Engine::Instance()->GetGraphicsEngine()->ActivateMaterial(material);
}

