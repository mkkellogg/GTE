#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <vector>
#include "forwardrendermanager.h"
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
#include "graphics/texture/texture.h"
#include "graphics/texture/textureattr.h"
#include "filesys/filesystem.h"
#include "asset/assetimporter.h"
#include "util/datastack.h"
#include "util/time.h"
#include "util/engineutility.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	* Single default constructor
	*/
	ForwardRenderManager::ForwardRenderManager()
	{
		lightCount = 0;
		ambientLightCount = 0;
		cameraCount = 0;
		renderQueueCount = 0;
		renderableSceneObjectCount = 0;
		forwardBlending = FowardBlendingMethod::Additive;
	}

	/*
	 * Clean up
	 */
	ForwardRenderManager::~ForwardRenderManager()
	{
		DestroyCachedShadowVolumes();
		DestroyRenderQueues();
	}

	/*
	 * Initialize. Return false if initialization false, true if it succeeds.
	 */
	Bool ForwardRenderManager::Init()
	{
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		AssetImporter assetImporter;
		ShaderSource shaderSource;

		// construct shadow volume material
		assetImporter.LoadBuiltInShaderSource("shadowvolume", shaderSource);
		shadowVolumeMaterial = objectManager->CreateMaterial("ShadowVolumeMaterial", shaderSource);		
		ASSERT(shadowVolumeMaterial.IsValid(), "RenderManager::Init -> Unable to create shadow volume material.");
		shadowVolumeMaterial->SetFaceCulling(RenderState::FaceCulling::None);
		shadowVolumeMaterial->SetDepthBufferWriteEnabled(false);

		// construct SSAO outline material
		assetImporter.LoadBuiltInShaderSource("ssaooutline", shaderSource);
		ssaoOutlineMaterial = objectManager->CreateMaterial("SSAOOutline", shaderSource);		
		ASSERT(ssaoOutlineMaterial.IsValid(), "RenderManager::Init -> Unable to create SSAO outline material.");
		ssaoOutlineMaterial->SetUseLighting(false);

		// construct depth-only material
		assetImporter.LoadBuiltInShaderSource("depthonly", shaderSource);
		depthOnlyMaterial = objectManager->CreateMaterial("DepthOnlyMaterial", shaderSource);		
		ASSERT(depthOnlyMaterial.IsValid(), "RenderManager::Init -> Unable to create depth only material.");
		depthOnlyMaterial->SetUseLighting(false);

		// construct depth-value material
		assetImporter.LoadBuiltInShaderSource("depthvalue", shaderSource);
		depthValueMaterial = objectManager->CreateMaterial("DepthValueMaterial", shaderSource);
		ASSERT(depthValueMaterial.IsValid(), "RenderManager::Init -> Unable to create depth value material.");
		depthValueMaterial->SetUseLighting(false);

		// build depth texture off-screen render target
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		RenderTargetSharedPtr defaultRenderTarget = graphics->GetDefaultRenderTarget();
		TextureAttributes colorTextureAttributes;
		colorTextureAttributes.Format = TextureFormat::R32F;
		colorTextureAttributes.FilterMode = TextureFilter::Point;
		colorTextureAttributes.WrapMode = TextureWrap::Clamp;
		depthRenderTarget = objectManager->CreateRenderTarget(true, true, false, colorTextureAttributes, defaultRenderTarget->GetWidth(), defaultRenderTarget->GetHeight());
		ASSERT(depthRenderTarget.IsValid(), "RenderManager::Init -> Unable to create off-screen rendering surface.");

		TextureSharedPtr depthTexture = depthRenderTarget->GetDepthTexture();
		TextureSharedPtr colorTexture = depthRenderTarget->GetColorTexture();

		ASSERT(colorTexture.IsValid(), "RenderManager::Init -> Unable to create off-screen color buffer.");
		ASSERT(depthTexture.IsValid(), "RenderManager::Init -> Unable to create off-screen depth buffer.");

		if (!InitFullScreenQuad())return false;

		return true;
	}

	/*
	 * Initialize the components needed to render a full screen quad.
	 */
	Bool ForwardRenderManager::InitFullScreenQuad()
	{
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		// create full screen quad mesh
		StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
		fullScreenQuad = EngineUtility::CreateRectangularMesh(meshAttributes, 1, 1, 1, 1, true, true, false);
		ASSERT(fullScreenQuad.IsValid(), "RenderManager::InitFullScreenQuad -> Unable to create full screen quad.");

		// transform full-screen quad to: X: [0..1], Y: [0..1]
		for (UInt32 i = 0; i < fullScreenQuad->GetSubMesh(0)->GetPostions()->GetCount(); i++)
		{
			Point3 * p = fullScreenQuad->GetSubMesh(0)->GetPostions()->GetPoint(i);
			p->x += 0.5;
			p->y += 0.5;
		}
		fullScreenQuad->Update();

		// create camera for rendering to [fullScreenQuad]
		fullScreenQuadCam = objectManager->CreateCamera();
		ASSERT(fullScreenQuadCam.IsValid(), "RenderManager::InitFullScreenQuad -> Unable to camera for full screen quad.");
		fullScreenQuadCam->SetProjectionMode(ProjectionMode::Orthographic);

		fullScreenQuadObject = objectManager->CreateSceneObject();
		ASSERT(fullScreenQuadObject.IsValid(), "RenderManager::InitFullScreenQuad -> Unable to scene object for full screen quad.");

		Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
		ASSERT(filter.IsValid(), "RenderManager::InitFullScreenQuad -> Unable to mesh filter for full screen quad.");

		filter->SetCastShadows(false);
		filter->SetReceiveShadows(false);
		filter->SetMesh3D(fullScreenQuad);
		fullScreenQuadObject->SetMesh3DFilter(filter);

		Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
		ASSERT(filter.IsValid(), "RenderManager::InitFullScreenQuad -> Unable to renderer for full screen quad.");
		fullScreenQuadObject->SetMesh3DRenderer(renderer);

		fullScreenQuadObject->SetActive(false);

		return true;
	}

	/*
	 * Look at the clear flags for a given camera and tell the graphics
	 * system to clear the corresponding buffers.
	 */
	void ForwardRenderManager::ClearBuffersForCamera(const Camera& camera) const
	{
		UInt32 clearBufferMask = camera.GetClearBufferMask();
		Engine::Instance()->GetGraphicsSystem()->ClearRenderBuffers(clearBufferMask);
	}

	/*
	 * Push a new render target on to the render target stack and activate it.
	 */
	void ForwardRenderManager::PushRenderTarget(RenderTargetRef renderTarget)
	{
		renderTargetStack.push(&renderTarget);
		// activate the new render target
		Engine::Instance()->GetGraphicsSystem()->ActivateRenderTarget(renderTarget);
	}

	/*
	 * Pop the current render target off the stack and activate the one below it.
	 * If there is none below, activate the default render target.
	 */
	RenderTargetRef ForwardRenderManager::PopRenderTarget()
	{
		const RenderTargetSharedPtr* old = nullptr;
		if (renderTargetStack.size() > 0)
		{
			old = renderTargetStack.top();
			renderTargetStack.pop();
		}

		if (renderTargetStack.size() > 0)
		{
			RenderTargetRef top = *renderTargetStack.top();
			// activate the new render target
			Engine::Instance()->GetGraphicsSystem()->ActivateRenderTarget(top);
		}
		else
		{
			// activate the default render target
			Engine::Instance()->GetGraphicsSystem()->RestoreDefaultRenderTarget();
		}
		
		return old == nullptr ? NullRenderTargetRef : *old;

	}

	/*
	 * Clear [renderTargetStack].
	 */
	void ForwardRenderManager::ClearRenderTargetStack()
	{
		renderTargetStack.empty();
	}

	/*
	 * Kick off rendering of the entire scene. This method first processes the scene hierarchy and
	 * stores a list of all cameras, lights, and meshes in the scene. After that it renders the scene
	 * from the perspective of each camera.
	 */
	void ForwardRenderManager::RenderScene()
	{
		// render the scene from the perspective of each camera found in ProcessScene()
		for (UInt32 i = 0; i < cameraCount; i++)
		{
			RenderSceneForCamera(i);
		}
	}

	/*
	 * Clear any caches that are set up for the render manager.
	 */
	void ForwardRenderManager::ClearCaches()
	{
		DestroyCachedShadowVolumes();
	}

	/*
	 * Render a quad-mesh that covers the entire screen and whose normal is orthogonal to the camera's
	 * direction vector. The vertices of the quad will be passed to the shader it the range:
	 *
	 *   X: [0 .. 1] From left to right
	 *   Y: [0 .. 1] From bottom to top
	 *
	 * [renderTarget] - The render target to which the quad should be rendered.
	 * [material] - The material (and shader) to be used for rendering.
	 * [clearBuffers] - Should the buffers belonging to [renderTarget] be cleared before rendering?
	 *
	 */
	void ForwardRenderManager::RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers)
	{
		Transform model;
		Transform modelView;
		Transform projection;
		Transform modelViewProjection;

		NONFATAL_ASSERT(renderTarget.IsValid(), "RenderManager::RenderFullScreenQuad -> Invalid render target.", true);
		NONFATAL_ASSERT(material.IsValid(), "RenderManager::RenderFullScreenQuad -> Invalid material.", true);

		// activate the material, which will switch the GPU's active shader to
		// the one associated with the material
		ActivateMaterial(material, false);

		// send uniforms set for the material to its shader
		SendActiveMaterialUniformsToShader();

		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();

		// save the currently active render target
		RenderTargetRef currentTarget = graphics->GetCurrrentRenderTarget();

		// set [renderTarget] as the current render target
		graphics->ActivateRenderTarget(renderTarget);

		// clear buffers (if necessary)
		if (clearBuffers)
		{
			IntMask clearMask = IntMaskUtil::CreateIntMask();
			IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Color);
			IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Depth);
			graphics->ClearRenderBuffers(clearMask);
		}

		fullScreenQuadObject->SetActive(true);

		// set [material] to be the material for the full screen quad mesh
		Mesh3DRendererRef renderer = fullScreenQuadObject->GetMesh3DRenderer();
		if (renderer->GetMaterialCount() > 0)renderer->SetMaterial(0, material);
		else renderer->AddMaterial(material);

		// render the full screen quad mesh
		for (UInt32 i = 0; i < fullScreenQuad->GetSubMeshCount(); i++)
		{
			renderer->GetSubRenderer(i)->Render();
		}
		fullScreenQuadObject->SetActive(false);

		// activate the old render target
		graphics->ActivateRenderTarget(currentTarget);
	}

	/*
	 * Kick off the scene processing from the root of the scene.
	 */
	void ForwardRenderManager::Update()
	{
		lightCount = 0;
		ambientLightCount = 0;
		cameraCount = 0;

		ClearAllRenderQueues();

		SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
		ASSERT(sceneRoot.IsValid(), "RenderManager::PreProcessScene -> 'sceneRoot' is null.");

		// gather information about the cameras, lights, and renderable meshes in the scene
		PreProcessScene(sceneRoot.GetRef(), 0);
		// perform any pre-transformations and calculations (e.g. vertex skinning)
		PreRenderScene();
		// calculate shadow volumes
		BuildSceneShadowVolumes();
	}

	/*
	 * PreProcessScene:
	 *
	 * For each SceneObject that is visited during this search, this method saves references to:
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
	void ForwardRenderManager::PreProcessScene(SceneObject& parent, UInt32 recursionDepth)
	{
		// enforce max recursion depth
		if (recursionDepth >= Constants::MaxObjectRecursionDepth - 1)return;

		for (UInt32 i = 0; i < parent.GetChildrenCount(); i++)
		{
			SceneObjectRef childRef = parent.GetChildAt(i);

			if (!childRef.IsValid())
			{
				Debug::PrintWarning("ForwardRenderManager::PreProcessScene -> Null scene object encountered.");
				continue;
			}

			SceneObject* child = childRef.GetPtr();

			// only process active scene objects
			if (child->IsActive())
			{
				CameraRef camera = child->GetCamera();
				if (camera.IsValid() && cameraCount < MAX_CAMERAS)
				{
					// add a scene camera from which to render the scene
					// always make sure to respect the render order index of the camera,
					// the array is sort in ascending order
					for (UInt32 i = 0; i <= cameraCount; i++)
					{
						if (i == cameraCount || sceneCameras[i]->GetCamera()->GetRenderOrderIndex() > camera->GetRenderOrderIndex())
						{
							for (UInt32 ii = cameraCount; ii > i; ii--)
								sceneCameras[ii] = sceneCameras[ii - 1];

							sceneCameras[i] = child;

							cameraCount++;
							break;
						}
					}
				}

				LightRef light = child->GetLight();
				if (light.IsValid())
				{
					if (light->GetType() == LightType::Ambient)
					{
						if (ambientLightCount < MAX_LIGHTS)
						{
							// add ambient light
							sceneAmbientLights[ambientLightCount] = child;
							ambientLightCount++;
						}
					}
					else
					{
						if (lightCount < MAX_LIGHTS)
						{
							// add non-ambient light
							sceneLights[lightCount] = child;
							lightCount++;
						}
					}
				}

				Mesh3DRendererRef meshRenderer = child->GetMesh3DRenderer();
				Mesh3DFilterRef meshFilter = child->GetMesh3DFilter();
				Mesh3DRef mesh = child->GetMesh3D();
				SkinnedMesh3DRendererRef skinnedMeshRenderer = child->GetSkinnedMesh3DRenderer();

				if(meshFilter.IsValid() && mesh.IsValid() && (meshRenderer.IsValid() || skinnedMeshRenderer.IsValid()))
				{
					Mesh3DRenderer * renderer = nullptr;
					if(meshRenderer.IsValid())
						renderer = meshRenderer.GetPtr();
					else if(skinnedMeshRenderer.IsValid())
						renderer = skinnedMeshRenderer.GetPtr();

					if(renderer != nullptr &&  mesh->GetSubMeshCount() == renderer->GetSubRendererCount() && renderer->GetMaterialCount() > 0 && renderableSceneObjectCount < MAX_SCENE_MESHES)
					{
						renderableSceneObjects[renderableSceneObjectCount] = child;
						renderableSceneObjectCount++;
						UInt32 materialCount = renderer->GetMaterialCount();
						UInt32 subMeshCount = mesh->GetSubMeshCount();
						for(UInt32 i = 0; i < subMeshCount; i++)
						{
							MaterialRef mat = renderer->GetMaterial(i % materialCount);
							RenderQueue* targetRenderQueue = GetRenderQueue((UInt32)mat->GetRenderQueueType());

							Transform * aggregateTransform = const_cast<Transform*>(&child->GetAggregateTransform());
							targetRenderQueue->Add(child, mesh->GetSubMesh(i).GetPtr(), renderer->GetSubRenderer(i).GetPtr(), &const_cast<MaterialSharedPtr&>(mat), meshFilter.GetPtr(), aggregateTransform);
						}
					}
				}

				// continue recursion through child object
				PreProcessScene(*child, recursionDepth + 1);
			}
		}
	}

	/*
	 * Iterate through all (active) mesh renderers in the scene all call their
	 * respective PreRender() methods. This is typically where vertex skinning will
	 * happen.
	 */
	void ForwardRenderManager::PreRenderScene()
	{
		Transform model;
		Transform modelInverse;

		// loop through each mesh-containing SceneObject in [sceneMeshObjects]
		for(UInt32 s = 0; s < renderableSceneObjectCount; s++)
		{
			SceneObject* child = renderableSceneObjects[s];

			model.SetTo(child->GetAggregateTransform());
			modelInverse.SetTo(model);
			modelInverse.Invert();

			Mesh3DRendererRef meshRenderer = child->GetMesh3DRenderer();
			SkinnedMesh3DRendererRef skinnedMeshRenderer = child->GetSkinnedMesh3DRenderer();

			if(meshRenderer.IsValid() || skinnedMeshRenderer.IsValid())
			{
				Mesh3DRenderer * renderer = nullptr;
				if(meshRenderer.IsValid())
					renderer = meshRenderer.GetPtr();
				else if(skinnedMeshRenderer.IsValid())
					renderer = skinnedMeshRenderer.GetPtr();

				if(renderer != nullptr)
				{
					// for each sub-renderer, call the PreRender() method
					for(UInt32 r = 0; r < renderer->GetSubRendererCount(); r++)
					{
						SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(r);
						if(subRenderer.IsValid())
						{
							subRenderer->PreRender(model.GetConstMatrix(), modelInverse.GetConstMatrix());
						}
					}
				}
			}
		}
	}

	/*
	* Empty out all render queues.
	*/
	void ForwardRenderManager::ClearAllRenderQueues()
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT(queue != nullptr, "RenderManager::ClearAllRenderQueues -> Null render queue encountered.", true);

			queue->Clear();
		}

		renderableSceneObjectCount = 0;
	}

	/*
	* Return the render queue that is linked to [renderQueueID]. If it doesn't
	* yet exist, create it.
	*/
	RenderQueue* ForwardRenderManager::GetRenderQueue(UInt32 renderQueueID)
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT_RTRN(queue != nullptr, "RenderManager::GetRenderQueue -> Null render queue encountered.", nullptr, true);

			if(queue->GetID() == renderQueueID)
			{
				return queue;
			}
		}
		ASSERT(renderQueueCount < MAX_RENDER_QUEUES, "RenderManager::GetRenderQueue -> Maximum number of render queues exceeded!");

		// create new queue for [renderQueueID], since it doesn't yet exist
		RenderQueue * newQueue = new(std::nothrow) RenderQueue(renderQueueID, 128, 128);
		ASSERT(newQueue != nullptr, "ForwardRenderManager::GetRenderQueue -> Unable to allocate to render queue.");
		renderQueues[renderQueueCount] = newQueue;
		renderQueueCount++;

		//TODO: sort more efficiently (quick sort would be best since it's in-place)
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			for(UInt32 j = 0; j < renderQueueCount; j++)
			{
				if(j < renderQueueCount - 1 && renderQueues[j] > renderQueues[j + 1])
				{
					RenderQueue * temp = renderQueues[j];
					renderQueues[j] = renderQueues[j + 1];
					renderQueues[j + 1] = temp;
				}
			}
		}

		return newQueue;
	}

	/*
	* Deallocate & destroy all render queues.
	*/
	void ForwardRenderManager::DestroyRenderQueues()
	{
		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			if(renderQueues[i] != nullptr)
			{
				delete renderQueues[i];
				renderQueues[i] = nullptr;
			}
		}
	}

	/*
	 * Render the entire scene from the perspective of a single camera. Uses [cameraIndex]
	 * as an index into the array of cameras [sceneCameras] that has been found by processing the scene.
	 */
	void ForwardRenderManager::RenderSceneForCamera(UInt32 cameraIndex)
	{
		NONFATAL_ASSERT(cameraIndex < cameraCount, "RenderManager::RenderSceneFromCamera -> cameraIndex out of bounds", true);

		SceneObject* object = sceneCameras[cameraIndex];
		NONFATAL_ASSERT(object != nullptr, "RenderManager::RenderSceneFromCamera -> Camera's scene object is not valid.", true);

		CameraRef cameraRef = object->GetCamera();
		NONFATAL_ASSERT(cameraRef.IsValid(), "RenderManager::RenderSceneFromCamera -> Camera is not valid.", true);
		Camera& camera = cameraRef.GetRef();

		SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
		ASSERT(sceneRoot.IsValid(), "RenderManager::RenderSceneFromCamera -> sceneRoot is null.");

		// currently we use forward rendering
		RenderSceneForCamera(camera);
	}

	/*
	 * This method will activate the render target belonging to [camera] (which may just be the default render target),
	 * and then pass control to ForwardRenderSceneForCameraAndCurrentRenderTarget.
	 */
	void ForwardRenderManager::RenderSceneForCamera(Camera& camera)
	{
		SceneObjectRef cameraObject = camera.GetSceneObject();
		NONFATAL_ASSERT(cameraObject.IsValid(), "RenderManager::ForwardRenderSceneForCamera -> Camera is not attached to a scene object.", true);

		// clear stack of activated render targets
		ClearRenderTargetStack();

		// activate camera's render target
		PushRenderTarget(camera.GetRenderTarget());

		// render the scene using the view transform of the current camera
		const Transform& cameraTransform = camera.GetSceneObject()->GetAggregateTransform();

		// we invert the camera's transform because at the shader level, the view transform is
		// really moving the world relative to the camera, rather than moving the camera
		// in the world
		Transform viewInverse;
		Transform viewTransform;
		viewTransform.SetTo(cameraTransform);
		viewInverse.SetTo(viewTransform);
		viewInverse.Invert();

		Graphics* graphics = Engine::Instance()->GetGraphicsSystem();

		// if the render target is a cube, render 6 times, twice for each axis
		if (camera.GetRenderTarget()->GetColorTexture().IsValid() &&
			camera.GetRenderTarget()->GetColorTexture()->GetAttributes().IsCube)
		{
			// front
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Front);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(0, 1, 0, 0, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
			// back
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Back);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(0, 1, 0, 180, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
			// top
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Top);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(1, 0, 0, 90, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
			// bottom
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Bottom);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(1, 0, 0, -90, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
			// left
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Left);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(0, 1, 0, 90, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
			// right
			graphics->ActivateCubeRenderTargetSide(CubeTextureSide::Right);
			viewInverse.SetTo(cameraTransform);
			viewInverse.Rotate(0, 1, 0, -90, true);
			viewInverse.Invert();
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
		}
		else
		{
			RenderSceneForCameraAndCurrentRenderTarget(camera, viewTransform, viewInverse);
		}

		RenderTargetRef copyTarget = camera.GetCopyRenderTarget();
		if (copyTarget.IsValid())
		{
			RenderTargetRef source = camera.GetRenderTarget();
			graphics->CopyBetweenRenderTargets(source, copyTarget);
		}

		PopRenderTarget();
	}

	/*
	 * Render all the meshes in the scene using forward rendering. The camera's inverted transform is used
	 * as the view transform. The reason the inverse is used is because on the GPU side of things the view
	 * transform is used to move the world relative to the camera, rather than move the camera in the world.
	 *
	 * Since this method uses a forward-rendering approach, each mesh is rendered once for each light and the output from
	 * each pass is combined with the others using additive blending. This method calls ForwardRenderSceneForLight(),
	 * which then calls ForwardRenderSceneObject(). The additive blending occurs in the ForwardRenderSceneObject() method.
	 *
	 * This method will render to whatever render target is currently active.
	 */
	void ForwardRenderManager::RenderSceneForCameraAndCurrentRenderTarget(Camera& camera, const Transform& viewTransform, const Transform& viewInverse)
	{
		// clear the list of objects that have been rendered at least once. this list is used to
		// determine if additive blending should be turned on or off. if an object is being rendered for the
		// first time, additive blending should be off; otherwise it should be on.
		renderedObjects.clear();

		// clear the appropriate render buffers for this camera
		ClearBuffersForCamera(camera);

		// modelPreTransform is pre-multiplied with the transform of each rendered scene object & light
		Transform modelPreTransform = camera.GetUniformWorldSceneObjectTransform();

		// we have not yet rendered any ambient lights
		Bool renderedAmbient = false;

		// loop through each ambient light and render the scene for that light
		if (camera.IsAmbientPassEnabled())
		{
			for (UInt32 l = 0; l < ambientLightCount; l++)
			{
				SceneObject* lightObject = sceneAmbientLights[l];
				ASSERT(lightObject != nullptr, "RenderManager::ForwardRenderSceneForCameraAndCurrentRenderTarget -> Ambient light's scene object is not valid.");

				LightRef lightRef = lightObject->GetLight();
				ASSERT(lightRef.IsValid(), "RenderManager::ForwardRenderSceneForCameraAndCurrentRenderTarget -> Ambient light is not valid.");

				// render all objects in the scene that have non self-lit materials
				RenderSceneForLight(lightRef.GetRef(), lightObject->GetAggregateTransform(), modelPreTransform, viewTransform, viewInverse, camera);
				renderedAmbient = true;
			}
		}

		// we need to fill the depth buffer with the scene to allow
		// for proper shadow volume rendering and depth-buffer culling
		if (camera.IsDepthPassEnabled())
		{
			RenderDepthBuffer(modelPreTransform, viewTransform, viewInverse, camera);
		}

		// perform the standard screen-space ambient occlusion pass
		if (renderedAmbient && camera.IsSSAOEnabled() && camera.GetSSAORenderMode() == SSAORenderMode::Standard)
		{
			RenderSceneSSAO(modelPreTransform, viewTransform, viewInverse, camera);
		}

		// loop through each regular light and render scene for that light
		for (UInt32 l = 0; l < lightCount; l++)
		{
			SceneObject* lightObject = sceneLights[l];
			ASSERT(lightObject != nullptr, "RenderManager::ForwardRenderSceneForCameraAndCurrentRenderTarget -> Light's scene object is not valid.");

			LightRef lightRef = lightObject->GetLight();
			ASSERT(lightRef.IsValid(), "RenderManager::ForwardRenderSceneForCameraAndCurrentRenderTarget -> Light is not valid.");

			// render all objects in the scene that have non self-lit materials
			RenderSceneForLight(lightRef.GetRef(), lightObject->GetAggregateTransform(), modelPreTransform, viewTransform, viewInverse, camera);
		}

		// perform the screen-space ambient occlusion pass as an outline effect
		if (camera.IsSSAOEnabled() && camera.GetSSAORenderMode() == SSAORenderMode::Outline)
		{
			RenderSceneSSAO(modelPreTransform, viewTransform, viewInverse, camera);
		}

		// render all self-lit objects in the scene once
		RenderSceneForSelfLitMaterials(modelPreTransform, viewTransform, viewInverse, camera);

		// if this camera has a skybox that is set up and enabled, then we want to render it
		if (camera.IsSkyboxSetup() && camera.IsSkyboxEnabled())
		{
			RenderSkyboxForCamera(camera, modelPreTransform, viewTransform, viewInverse);
		}
	}

	/*
	 * Forward-Render the scene for a single light [light] from the perspective
	 * of [camera] using [viewTransform] as the camera's position and orientation.
	 *
	 * This method performs two passes:
	 *
	 * Pass 1: If [light] is not ambient, render shadow volumes for all meshes in the scene for [light] into the stencil buffer.
	 * Pass 2: Perform actual rendering of all meshes in the scene for [light]. If [light] is not ambient, this pass will
	 *         exclude screen pixels that are hidden from [light] based on the stencil buffer contents from pass 0. Is [light]
	 *         is ambient, then this pass will perform a standard render of all meshes in the scene.
	 *
	 * [modelPreTransform] is pre-multiplied with the transform of each rendered scene object & light
	 */

	void ForwardRenderManager::RenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& modelPreTransform,
		const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera)
	{
		Transform modelView;
		Transform model;
		Transform modelInverse;

		Transform lightTransform = lightFullTransform;
		lightTransform.PreTransformBy(modelPreTransform);

		Point3 lightPosition;
		lightTransform.TransformPoint(lightPosition);

		Vector3 lightDirection = light.GetDirection();
		lightTransform.TransformVector(lightDirection);

		RenderMode currentRenderMode = RenderMode::None;

		enum RenderPass
		{
			ShadowVolumeRender = 0,
			StandardRender = 1,
			_PassCount = 2
		};

		for (Int32 pass = 0; pass < RenderPass::_PassCount; pass++)
		{
			if (pass == ShadowVolumeRender) // shadow volume pass
			{
				// check if this light can cast shadows; if not we skip this pass
				if (light.GetShadowsEnabled() && light.GetType() != LightType::Ambient)
				{
					currentRenderMode = RenderMode::ShadowVolumeRender;
					Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::ShadowVolumeRender);
				}
				else
					continue;
			}

			for(UInt32 i = 0; i < renderQueueCount; i++)
			{
				RenderQueue* queue = renderQueues[i];
				NONFATAL_ASSERT(queue != nullptr, "RenderManager::RenderSceneForLight -> Null render queue encountered.", true);

				// loop through each mesh-containing SceneObject in [sceneMeshObjects]
				for(UInt32 s = 0; s < queue->GetObjectCount(); s++)
				{
					RenderQueueEntry* entry = queue->GetObject(s);
					NONFATAL_ASSERT(entry != nullptr, "RenderManager::RenderSceneForLight -> Null render queue entry encountered.", true);

					SceneObject* sceneObject = entry->Container;
					NONFATAL_ASSERT(sceneObject != nullptr, "RenderManager::RenderSceneForLight -> Null scene object encountered.", true);

					Mesh3DFilterRef filter = sceneObject->GetMesh3DFilter();

					// copy the full transform of the scene object, including those of all ancestors
					Transform full;
					full.SetTo(sceneObject->GetAggregateTransform());

					// make sure the current mesh should not be culled from [light].
					if(!ShouldCullFromCamera(camera, *sceneObject) &&
					   !ShouldCullFromLight(light, lightPosition, full, *sceneObject))
					{
						if(pass == ShadowVolumeRender) // shadow volume pass
						{
							if(filter->GetCastShadows())
							{
								RenderShadowVolumeForMesh(*entry, light, lightPosition, lightDirection, modelPreTransform, viewTransformInverse, camera);
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
									Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::StandardWithShadowVolumeTest);
								}
							}
							else if(currentRenderMode != RenderMode::Standard)
							{
								currentRenderMode = RenderMode::Standard;
								Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
							}

							// set up lighting descriptor for non self-lit lighting
							LightingDescriptor lightingDescriptor(&light, &lightPosition, &lightDirection, true);
							RenderMesh(*entry, lightingDescriptor, modelPreTransform, viewTransform, viewTransformInverse,
											  camera, NullMaterialRef, true, true, FowardBlendingFilter::OnlyIfRendered);
						}
					}
				}
			}
		}
	}

	/*
	 * Forward-Render all the meshes found in ProcessScene() that have self-lit materials from the perspective
	 * of [camera] using [viewTransform] as the camera's position and orientation.
	 *
	 * [modelPreTransform] is pre-multiplied with the transform of each rendered scene object.
	 */
	void ForwardRenderManager::RenderSceneForSelfLitMaterials(const Transform& modelPreTransform, const Transform& viewTransform,
		const Transform& viewTransformInverse, const Camera& camera)
	{
		RenderSceneWithSelfLitLighting(modelPreTransform, viewTransform, viewTransformInverse, camera, NullMaterialRef, true, true, FowardBlendingFilter::OnlyIfRendered);
	}

	/*
	 * Render the skybox for [camera] using [viewTransformInverse] as the view transformation.
	 * [viewTransformation] should be the inverse of the camera's transformation.
	 */
	void ForwardRenderManager::RenderSkyboxForCamera(Camera& camera, const Transform& modelPreTransform,  const Transform& viewTransform, const Transform& viewTransformInverse)
	{
		Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);

		// ensure the camera has a valid skybox
		if (camera.IsSkyboxSetup() && camera.IsSkyboxEnabled())
		{
			SceneObjectRef cameraObject = camera.GetSceneObject();
			NONFATAL_ASSERT(cameraObject.IsValid(), "RenderManager::RenderSkyboxForCamera -> Camera is not attached to a scene object.", true);

			// retrieve the scene objects for the camera and for the camera's skybox
			SceneObjectRef skyboxObject = camera.GetSkyboxSceneObject();
			NONFATAL_ASSERT(skyboxObject.IsValid(), "RenderManager::RenderSkyboxForCamera -> Camera has invalid skybox scene object.", true);

			Mesh3DRendererRef meshRenderer = skyboxObject->GetMesh3DRenderer();
			SkinnedMesh3DRendererRef skinnedmeshRenderer = skyboxObject->GetSkinnedMesh3DRenderer();
			Mesh3DRenderer * renderer = nullptr;
			if(meshRenderer.IsValid())
				renderer = meshRenderer.GetPtr();
			else if(skinnedmeshRenderer.IsValid())
				renderer = skinnedmeshRenderer.GetPtr();

			NONFATAL_ASSERT(renderer != nullptr, "RenderManager::RenderSkyboxForCamera -> Could not find valid renderer for skybox.", true);
		
			Mesh3DRef mesh = renderer->GetTargetMesh();
			NONFATAL_ASSERT(mesh.IsValid(), "RenderManager::RenderSkyboxForCamera -> Skybox has invalid mesh.", true);
			NONFATAL_ASSERT(mesh->GetSubMeshCount() > 0, "RenderManager::RenderSkyboxForCamera -> Skybox has empty mesh.", true);
			NONFATAL_ASSERT(renderer->GetSubRendererCount() > 0, "RenderManager::RenderSkyboxForCamera -> Skybox has no renderers.", true);
			NONFATAL_ASSERT(renderer->GetMaterialCount() > 0, "RenderManager::RenderSkyboxForCamera -> Skybox has no material.", true);
			
			skyboxObject->SetActive(true);

			skyboxEntry.Container = skyboxObject.GetPtr();
			skyboxEntry.Mesh = mesh->GetSubMesh(0).GetPtr();
			skyboxEntry.Renderer = renderer->GetSubRenderer(0).GetPtr();
			skyboxEntry.RenderMaterial = const_cast<MaterialSharedPtr*>(&renderer->GetMaterial(0));

			LightingDescriptor lightingDescriptor(nullptr, nullptr, nullptr, false);

			// render the skybox
			RenderMesh(skyboxEntry, lightingDescriptor, modelPreTransform, viewTransform, viewTransformInverse, camera, NullMaterialRef, true, true, FowardBlendingFilter::Never);

			skyboxObject->SetActive(false);
		}
	}

	/*
	 * Render the scene to only the depth-buffer. Render from the perspective
	 * of [camera] using [viewTransform] as the camera's position and orientation.
	 *
	 * [modelPreTransform] is pre-multiplied with the transform of each rendered scene object.
	 */
	void ForwardRenderManager::RenderDepthBuffer(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera)
	{
		RenderSceneWithSelfLitLighting(modelPreTransform, viewTransform, viewTransformInverse, camera, depthOnlyMaterial, false, false, FowardBlendingFilter::Never);
	}

	/*
	 * Render the screen-space ambient occlusion for the scene. Render from the perspective
	 * of [camera] using [viewTransform] as the camera's position and orientation.
	 *
	 * [modelPreTransform] is pre-multiplied with the transform of each rendered scene object.
	 */
	void ForwardRenderManager::RenderSceneSSAO(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera)
	{
		Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
		GraphicsAttributes attributes = Engine::Instance()->GetGraphicsSystem()->GetAttributes();

		// activate the off-screen render target
		PushRenderTarget(depthRenderTarget);

		// clear the relevant buffers in the off-screen render target
		IntMask clearMask = IntMaskUtil::CreateIntMask();
		if (depthRenderTarget->HasBuffer(RenderBufferType::Color))IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Color);
		if (depthRenderTarget->HasBuffer(RenderBufferType::Depth))IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Depth);
		Engine::Instance()->GetGraphicsSystem()->ClearRenderBuffers(clearMask);

		// render the depth values for the scene to the off-screen color texture (filter out non-static objects)
		MaterialRef depthValueMaterialRef = depthValueMaterial;
		RenderSceneWithSelfLitLighting(modelPreTransform, viewTransform, viewTransformInverse, camera, depthValueMaterialRef, false, true, FowardBlendingFilter::Never, [=](SceneObject* sceneObject)
		{
			return !sceneObject->IsStatic();
		});
		// restore previous render target
		PopRenderTarget();

		Matrix4x4 projectionInvMat;
		camera.GetInverseProjectionTransform().CopyMatrix(projectionInvMat);

		// retrieve the color texture (which contains depth values) from the off-screen render target
		TextureRef depthTexture = depthRenderTarget->GetColorTexture();

		// set SSAO material values
		ssaoOutlineMaterial->SetTexture(depthTexture, "DEPTH_TEXTURE");
		ssaoOutlineMaterial->SetMatrix4x4(projectionInvMat, "INV_PROJECTION_MATRIX");
		ssaoOutlineMaterial->SetUniform1f(.5f, "DISTANCE_THRESHHOLD");
		ssaoOutlineMaterial->SetUniform2f(.3f, .75f, "FILTER_RADIUS");
		ssaoOutlineMaterial->SetUniform1f((GTE::Real)depthRenderTarget->GetWidth(), "SCREEN_WIDTH");
		ssaoOutlineMaterial->SetUniform1f((GTE::Real)depthRenderTarget->GetHeight(), "SCREEN_HEIGHT");

		FowardBlendingMethod currentFoward = GetForwardBlending();

		// set forward rendering blending to subtractive since we need to darken areas of the
		// scene that are occluded.
		SetForwardBlending(FowardBlendingMethod::Subtractive);
		// rendering scene with SSAO material and filter out non-static objects
		MaterialRef ssaoOutlineMaterialRef = ssaoOutlineMaterial;
		RenderSceneWithSelfLitLighting(modelPreTransform, viewTransform, viewTransformInverse, camera, ssaoOutlineMaterialRef, false, true, FowardBlendingFilter::Always, [=](SceneObject* sceneObject)
		{
			return !sceneObject->IsStatic();
		});
		// restore previous forward rendering blend mode
		SetForwardBlending(currentFoward);
	}

	/*
	 * Same as main ForwardRenderSceneWithSelfLitLighting() method, except it passes nullptr
	 * as the value for [filterFunction].
	 *
	 * [modelPreTransform] is pre-multiplied with the transform of each rendered scene object & light.
	 */
	void ForwardRenderManager::RenderSceneWithSelfLitLighting(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse,
		const Camera& camera, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter)
	{
		RenderSceneWithSelfLitLighting(modelPreTransform, viewTransform, viewTransformInverse, camera, material, flagRendered, renderMoreThanOnce, blendingFilter, nullptr);
	}

	/*
	 * Render all objects in the scene with a self-lit lighting descriptor. Use [material] as the
	 * override material (if it is valid, needs to be self-lit), otherwise all meshes will be rendered
	 * with their own materials (if they are self-lit).
	 *
	 * [modelPreTransform] - This transform is pre-multiplied with the transform of each rendered scene object.
	 * [viewTransform] - The transform of the viewing camera.
	 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
	 * [camera] - The Camera object for which rendering is taking place.
	 * [material] - If this is valid, it will be used to render all meshes.
	 * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
	 *                  works for that mesh on future rendering passes.
	 * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
	 * [blendingFilter] - Determines how forward-rendering blending will be applied.
	 * [filterFunction] - Used to filter out select scene objects.
	 *
	 * Render from the perspective of [camera] using [viewTransformInverse] as the camera's position and orientation.
	 */
	void ForwardRenderManager::RenderSceneWithSelfLitLighting(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse,
		const Camera& camera, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter,
		std::function<Bool(SceneObject*)> filterFunction)
	{
		Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);

		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT(queue != nullptr, "RenderManager::RenderSceneWithSelfLitLighting -> Null render queue encountered.", true);

			// loop through each mesh-containing SceneObject in [sceneMeshObjects]
			for(UInt32 s = 0; s < queue->GetObjectCount(); s++)
			{
				RenderQueueEntry* entry = queue->GetObject(s);
				SceneObject* sceneObject = entry->Container;

				// check if [childRef] should be rendered for [camera]
				if(ShouldCullFromCamera(camera, *sceneObject))
				{
					continue;
				}

				// execute filter function (if one is specified)
				if(filterFunction != nullptr)
				{
					Bool filter = filterFunction(sceneObject);
					if(filter)continue;
				}

				// copy the full transform of the scene object, including those of all ancestors
				Transform full;
				full.SetTo(sceneObject->GetAggregateTransform());

				LightingDescriptor lightingDescriptor(nullptr, nullptr, nullptr, false);
				RenderMesh(*entry, lightingDescriptor, modelPreTransform, viewTransform, viewTransformInverse, camera, material, flagRendered, renderMoreThanOnce, blendingFilter);
			}
		}
	}
	/*
	 * Forward-Render the mesh attached to [entry].
	 *
	 * [entry] - RenderQueueEntry object that contains the relevant renderable and transform information.
	 * [lightingDescriptor] - Describes the lighting to be used for rendering (if there is any).
	 * [camera] - The Camera object for which rendering is taking place.
	 * [modelPreTransform] - This transform is pre-multiplied with the transform of each rendered scene object.
	 * [viewTransform] - The transform of the viewing camera.
	 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
	 * [materialOverride] - If this is valid, it will be used to render all meshes.
	 * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
	 *                  works for that mesh on future rendering passes.
	 * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
	 * [blendingFilter] - Determines how forward-rendering blending will be applied.
	 */
	void ForwardRenderManager::RenderMesh(RenderQueueEntry& entry, const LightingDescriptor& lightingDescriptor, const Transform& modelPreTransform,
		const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera, MaterialRef  materialOverride,
		Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter)
	{
		Transform modelViewProjection;
		Transform modelView;
		Transform model;
	
		SceneObject* sceneObject = entry.Container;
		SubMesh3DRenderer* renderer = entry.Renderer;
		SubMesh3D* mesh = entry.Mesh;

		NONFATAL_ASSERT(mesh != nullptr, "RenderManager::RenderMesh -> Renderer returned null mesh.", true);
		NONFATAL_ASSERT(sceneObject != nullptr, "RenderManager::RenderMesh -> Scene object is not valid.", true);
		NONFATAL_ASSERT(renderer != nullptr, "RenderManager::RenderMesh -> Null sub renderer encountered.", true);

		// determine if this mesh has been rendered before
		ObjectPairKey key(sceneObject->GetObjectID(), mesh->GetObjectID());
		Bool rendered = renderedObjects[key];
		
		// if we have an override material, we use that for every mesh
		Bool doMaterialOvverride = materialOverride.IsValid() ? true : false;
		MaterialRef currentMaterial = doMaterialOvverride ? materialOverride : *entry.RenderMaterial;

		NONFATAL_ASSERT(currentMaterial != nullptr, "RenderManager::RenderMesh -> Null material encountered.", true);

		Bool skipMesh = false;
		// current material is self-lit, the we only want to render if the
		//lighting descriptor specifies self-lit and vice-versa
		if(currentMaterial->UseLighting() != lightingDescriptor.UseLighting)skipMesh = true;
		if(rendered && !renderMoreThanOnce)skipMesh = true;

		if(skipMesh)return;

		model.SetTo(sceneObject->GetAggregateTransform());
		model.PreTransformBy(modelPreTransform);

		// concatenate model transform with inverted view transform, and then with
		// the camera's projection transform.
		modelView.SetTo(model);
		modelView.PreTransformBy(viewTransformInverse);
		modelViewProjection.SetTo(modelView);
		modelViewProjection.PreTransformBy(camera.GetProjectionTransform());

		// activate the material, which will switch the GPU's active shader to
		// the one associated with the material
		ActivateMaterial(currentMaterial, camera.GetReverseCulling());

		// send uniforms set for the new material to its shader
		SendActiveMaterialUniformsToShader();

		// send light data to the active shader (if it needs it)
		if (lightingDescriptor.UseLighting)
		{
			currentMaterial->SendLightToShader(lightingDescriptor.LightObject, lightingDescriptor.LightPosition, lightingDescriptor.LightDirection);
		}
		// pass concatenated modelViewTransform and projection transforms to shader
		SendTransformUniformsToShader(model, modelView, viewTransformInverse, camera.GetProjectionTransform(), modelViewProjection);

		Point3 viewOrigin;
		viewTransform.TransformPoint(viewOrigin);
		// send camera attributes to the active shader
		SendCameraAttributesToShader(camera, viewOrigin);

		// if this sub mesh has already been rendered by this camera, then we want to use
		// additive blending to combine it with the output from other lights. Otherwise
		// turn off blending and render.
		if ((rendered && blendingFilter == FowardBlendingFilter::OnlyIfRendered) || (blendingFilter == FowardBlendingFilter::Always))
		{
			if (GetForwardBlending() == FowardBlendingMethod::Subtractive)
			{
				Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(true);
				Engine::Instance()->GetGraphicsSystem()->SetBlendingFunction(RenderState::BlendingMethod::Zero, RenderState::BlendingMethod::SrcAlpha);
			}
			else
			{
				Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(true);
				Engine::Instance()->GetGraphicsSystem()->SetBlendingFunction(RenderState::BlendingMethod::One, RenderState::BlendingMethod::One);
			}
		}
		else
		{
			Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(false);
		}

		// render the current mesh
		renderer->Render();

		// flag the current scene object as being rendered (at least once)
		if (flagRendered)renderedObjects[key] = true;

	}

	/*
	 * Render the shadow volumes for the meshes attached to [sceneObject] for [light]. This essentially means altering the
	 * stencil buffer to reflect areas of the rendered scene that are shadowed from [light] by the mesh attached to [entry].
	 *
	 * [entry] - RenderQueueEntry object that contains the relevant renderable and transform information.
	 * [lightPosition] - The world space position of [light].
	 * [camera] - The Camera object for which rendering is taking place.
	 * [modelPreTransform] - This transform is pre-multiplied with the transform of each rendered shadow volume.
	 * [viewTransformInverse] - The view transform for rendering. (it should be the inverse of the camera's local-to-world-space transformation).
	 */
	void ForwardRenderManager::RenderShadowVolumeForMesh(RenderQueueEntry& entry, const Light& light, const Point3& lightPosition, const Vector3& lightDirection,
		const Transform& modelPreTransform, const Transform& viewTransformInverse, const Camera& camera)
	{
		Transform modelViewProjection;
		Transform modelView;
		Transform model;
		Transform modelInverse;

		SceneObject* sceneObject = entry.Container;
		SubMesh3DRenderer* renderer = entry.Renderer;
		SubMesh3D* mesh = entry.Mesh;

		NONFATAL_ASSERT(mesh != nullptr, "RenderManager::RenderShadowVolumesForSceneObject -> Renderer returned null mesh.", true);
		NONFATAL_ASSERT(sceneObject != nullptr, "RenderManager::RenderShadowVolumesForSceneObject -> Scene object is not valid.", true);
		NONFATAL_ASSERT(renderer != nullptr, "RenderManager::RenderShadowVolumesForSceneObject -> Null sub renderer encountered.", true);

		// if mesh doesn't have face data, it can't have a shadow volume
		if(!mesh->HasFaces())return;

		Mesh3DFilter* filter = entry.MeshFilter;
		NONFATAL_ASSERT(filter != nullptr, "RenderManager::RenderShadowVolumesForSceneObject -> Scene object has null mesh filter.", true);

		// calculate model transform and inverse model transform
		model.SetTo(sceneObject->GetAggregateTransform());
		model.PreTransformBy(modelPreTransform);
		modelInverse.SetTo(model);
		modelInverse.Invert();

		// calculate the position and/or direction of [light]
		// in the mesh's local space
		Point3 modelLocalLightPos = lightPosition;
		Vector3 modelLocalLightDir = lightDirection;
		modelInverse.TransformPoint(modelLocalLightPos);
		modelInverse.TransformVector(modelLocalLightDir);


		// build special MVP transform for rendering shadow volumes
		Real scaleFactor = filter->GetUseBackSetShadowVolume() ? .99f : .99f;
		BuildShadowVolumeMVPTransform(model, camera, viewTransformInverse, modelViewProjection, scaleFactor, scaleFactor);

		// activate the material, which will switch the GPU's active shader to
		// the one associated with the material
		ActivateMaterial(shadowVolumeMaterial, camera.GetReverseCulling());

		if (filter->GetUseCustomShadowVolumeOffset())
		{
			// set the epsilon offset for the shadow volume shader based on custom value
			shadowVolumeMaterial->SetUniform1f(filter->GetCustomShadowVolumeOffset(), "EPSILON");
		}
		else
		{
			// set the epsilon offset for the shadow volume shader based on type of shadow volume
			if (filter->GetUseBackSetShadowVolume())shadowVolumeMaterial->SetUniform1f(.00002f, "EPSILON");
			else shadowVolumeMaterial->SetUniform1f(.2f, "EPSILON");
		}

		// send uniforms set for the shadow volume material to its shader
		SendActiveMaterialUniformsToShader();
		// pass special shadow volume model-view-matrix to shader
		SendModelViewProjectionToShader(modelViewProjection);

		Point3 viewOrigin;
		// send camera data to shader for [camera]
		SendCameraAttributesToShader(camera, viewOrigin);

		// send shadow volume uniforms to shader
		shadowVolumeMaterial->SendLightToShader(&light, &modelLocalLightPos, &modelLocalLightDir);

		// form cache key from sub-renderer's object ID and light's object ID
		ObjectPairKey cacheKey;
		cacheKey.ObjectAID = renderer->GetObjectID();
		cacheKey.ObjectBID = light.GetObjectID();

		const Point3Array * cachedShadowVolume = nullptr;
		cachedShadowVolume = GetCachedShadowVolume(cacheKey);

		// render the shadow volume if it is valid
		if (cachedShadowVolume != nullptr)
		{
			renderer->RenderShadowVolume(cachedShadowVolume);
		}
	}

	/*
	 * Build shadow volumes for all relevant meshes for all shadow casting lights.
	 */
	void ForwardRenderManager::BuildSceneShadowVolumes()
	{
		// loop through each light in [sceneLights]
		for (UInt32 l = 0; l < lightCount; l++)
		{
			SceneObject* lightObject = sceneLights[l];
			NONFATAL_ASSERT(lightObject != nullptr, "RenderManager::BuildSceneShadowVolumes -> Light's scene object is not valid.", true);

			// verify the light is active
			if (lightObject->IsActive())
			{
				LightRef lightRef = lightObject->GetLight();
				NONFATAL_ASSERT(lightRef.IsValid(), "RenderManager::BuildSceneShadowVolumes -> Light is not valid.", true);

				// verify that this light casts shadows
				if (lightRef->GetShadowsEnabled())
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
	void ForwardRenderManager::BuildShadowVolumesForLight(const Light& light, const Transform& lightFullTransform)
	{
		Transform modelView;
		Transform model;
		Transform modelInverse;

		Point3 lightPosition;
		lightFullTransform.TransformPoint(lightPosition);

		Vector3 lightDirection = light.GetDirection();
		lightFullTransform.TransformVector(lightDirection);


		for(UInt32 i = 0; i < renderQueueCount; i++)
		{
			RenderQueue* queue = renderQueues[i];
			NONFATAL_ASSERT(queue != nullptr, "RenderManager::BuildShadowVolumesForLight -> Null render queue encountered.", true);

			// loop through each mesh-containing SceneObject in [sceneMeshObjects]
			for(UInt32 s = 0; s < queue->GetObjectCount(); s++)
			{
				RenderQueueEntry* entry = queue->GetObject(s);
				NONFATAL_ASSERT(entry != nullptr, "RenderManager::BuildShadowVolumesForLight -> Null render queue entry encountered.", true);

				SceneObject* sceneObject = entry->Container;

				if(sceneObject->IsActive())
				{
					// copy the full transform of the scene object, including those of all ancestors
					Transform full;
					full.SetTo(sceneObject->GetAggregateTransform());

					// make sure the current mesh should not be culled from [light].
					if(!ShouldCullFromLight(light, lightPosition, full, *sceneObject))
					{
						BuildShadowVolumesForSceneObject(*sceneObject, light, lightPosition, lightDirection);
					}
				}
			}
		}
	}

	/*
	 * Build (and cache) shadow volumes for the meshes attached to [sceneObject] for [light], using
	 * [lightPosition] as the light's world position.
	 */
	void ForwardRenderManager::BuildShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Vector3& lightDirection)
	{
		Mesh3DRenderer * renderer = nullptr;
		Transform modelViewProjection;
		Transform modelView;
		Transform model;
		Transform modelInverse;

		Mesh3DRendererRef mesh3DRenderer = sceneObject.GetMesh3DRenderer();
		SkinnedMesh3DRendererRef skinnedMesh3DRenderer = sceneObject.GetSkinnedMesh3DRenderer();

		// check if [sceneObject] has a mesh & renderer
		if (mesh3DRenderer.IsValid())
		{
			renderer = const_cast<Mesh3DRenderer *>(mesh3DRenderer.GetConstPtr());
		}
		else if (skinnedMesh3DRenderer.IsValid())
		{
			renderer = const_cast<SkinnedMesh3DRenderer *>(skinnedMesh3DRenderer.GetConstPtr());
		}

		if (renderer != nullptr)
		{
			Mesh3DRef mesh = renderer->GetTargetMesh();
			Mesh3DFilterRef filter = sceneObject.GetMesh3DFilter();

			NONFATAL_ASSERT(mesh.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> Renderer returned null mesh.", true);
			NONFATAL_ASSERT(filter.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> Scene object has null mesh filter.", true);
			NONFATAL_ASSERT(mesh->GetSubMeshCount() == renderer->GetSubRendererCount(), "RenderManager::BuildShadowVolumesForSceneObject -> Sub mesh count does not match sub renderer count!.", true);
			NONFATAL_ASSERT(renderer->GetMaterialCount() > 0, "RenderManager::BuildShadowVolumesForSceneObject -> Renderer has no materials.", true);

			// calculate model transform and inverse model transform
			model.SetTo(sceneObject.GetAggregateTransform());
			modelInverse.SetTo(model);
			modelInverse.Invert();

			// calculate the position and/or direction of [light]
			// in the mesh's local space
			Point3 modelLocalLightPos = lightPosition;
			Vector3 modelLocalLightDir = lightDirection;
			modelInverse.TransformPoint(modelLocalLightPos);
			modelInverse.TransformVector(modelLocalLightDir);

			// loop through each sub-renderer and render the shadow volume for its sub-mesh
			for (UInt32 i = 0; i < renderer->GetSubRendererCount(); i++)
			{
				SubMesh3DRendererRef subRenderer = renderer->GetSubRenderer(i);
				SubMesh3DRef subMesh = mesh->GetSubMesh(i);

				// if mesh doesn't have face data, it can't have a shadow volume
				if (!subMesh->HasFaces())continue;

				NONFATAL_ASSERT(subRenderer.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> Null sub renderer encountered.", true);
				NONFATAL_ASSERT(subMesh.IsValid(), "RenderManager::BuildShadowVolumesForSceneObject -> Null sub mesh encountered.", true);

				Vector3 lightPosDir;
				if (light.GetType() == LightType::Directional)
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
				Bool cached = false;

				// a shadow volume is dynamic if the light's scene object is not static or the mesh's
				// scene object is not static
				Bool dynamic = !(sceneObject.IsStatic()) || !(lightObject->IsStatic());

				// form cache key from sub-renderer's object ID and light's object ID
				cacheKey.ObjectAID = subRenderer->GetObjectID();
				cacheKey.ObjectBID = light.GetObjectID();

				const Point3Array * cachedShadowVolume = GetCachedShadowVolume(cacheKey);

				// check if this shadow volume is already cached
				if (cachedShadowVolume != nullptr)
				{
					cached = true;
				}

				if (!cached || dynamic) // always rebuild dynamic shadow volumes
				{
					// calculate shadow volume geometry
					subRenderer->BuildShadowVolume(lightPosDir, light.GetType() == LightType::Directional || light.GetType() == LightType::Planar, filter->GetUseBackSetShadowVolume());

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
	Bool ForwardRenderManager::ValidateSceneObjectForRendering(SceneObjectRef sceneObject) const
	{
		if (!sceneObject.IsValid())
		{
			Debug::PrintWarning("RenderManager::ValidateSceneObjectForRendering -> Null scene object encountered.");
			return false;
		}

		if (!sceneObject->IsActive())return false;

		Mesh3DRenderer * renderer = nullptr;
		Mesh3DRendererRef mesh3DRenderer = sceneObject->GetMesh3DRenderer();
		SkinnedMesh3DRendererRef skinnedMesh3DRenderer = sceneObject->GetSkinnedMesh3DRenderer();

		// check if [sceneObject] has a mesh & renderer
		if(mesh3DRenderer.IsValid())
		{
			renderer = const_cast<Mesh3DRenderer *>(mesh3DRenderer.GetConstPtr());
		}
		else if(skinnedMesh3DRenderer.IsValid())
		{
			renderer = const_cast<SkinnedMesh3DRenderer *>(skinnedMesh3DRenderer.GetConstPtr());
		}

		Mesh3DRef mesh = renderer->GetTargetMesh();

		if (!mesh.IsValid())
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
	 * Multiplying the mesh geometry by this rotation matrix and then scaling X & Y ever so slightly has an
	 * effect similar to 'narrowing' the shadow volume around the mesh-to-light vector. This mitigates artifacts where
	 * the shadow volume's sides are very close to and parallel to mesh polygons and Z-fighting occurs.
	 *
	 * [modelTransform] - The transform from model space to world space.
	 * [camera] - The camera for which the scene is being rendered.
	 * [viewTransformInverse] - The inverse of the view transform.
	 * [outTransform] - The output model-view-projection Transform.
	 * [xScale] - factor by which to scale the shadow volume along the x-axis.
	 * [yScale] - factor by which to scale the shadow volume along the y-axis.
	 */
	void ForwardRenderManager::BuildShadowVolumeMVPTransform(const Transform& modelTransform, const Camera& camera, const Transform& viewTransformInverse,
													  Transform& outTransform, Real xScale, Real yScale) const
	{
		Transform modelView;
		Transform shadowVolumeViewTransform;

		shadowVolumeViewTransform.Scale(xScale, yScale, 1.00, true);
		modelView.SetTo(shadowVolumeViewTransform);
		modelView.PreTransformBy(modelTransform);
		modelView.PreTransformBy(viewTransformInverse);
		outTransform.SetTo(modelView);
		outTransform.PreTransformBy(camera.GetProjectionTransform());
	}

	/*
	 * Store a copy of a shadow volume in [shadowVolumeCache], keyed by [key].
	 */
	void ForwardRenderManager::CacheShadowVolume(const ObjectPairKey& key, const Point3Array * shadowVolume)
	{
		NONFATAL_ASSERT(shadowVolume != nullptr, "RenderManager::CacheShadowVolume -> Shadow volume is null.", true);

		Bool needsInit = false;
		Point3Array * target = nullptr;

		if (!HasCachedShadowVolume(key))
		{
			needsInit = true;
		}
		else
		{
			target = shadowVolumeCache[key];
			if (target->GetReservedCount() != shadowVolume->GetReservedCount())
			{
				ClearCachedShadowVolume(key);
				needsInit = true;
			}
		}

		if (needsInit)
		{
			target = new(std::nothrow) Point3Array();
			ASSERT(target != nullptr, "RenderManager::CacheShadowVolume -> Unable to allocate shadow volume copy.");

			Bool initSuccess = target->Init(shadowVolume->GetReservedCount());
			ASSERT(initSuccess, "RenderManager::CacheShadowVolume -> Unable to initialize shadow volume copy.");

			target->SetCount(shadowVolume->GetCount());
			shadowVolumeCache[key] = target;
		}

		target->SetCount(shadowVolume->GetCount());
		shadowVolume->CopyTo(target);
	}

	/*
	 * Remove the shadow volume cached for [key] (if it exists).
	 */
	void ForwardRenderManager::ClearCachedShadowVolume(const ObjectPairKey& key)
	{
		if (HasCachedShadowVolume(key))
		{
			Point3Array* shadowVolume = shadowVolumeCache[key];
			shadowVolumeCache.erase(key);
			if (shadowVolume != nullptr)
			{
				delete shadowVolume;
			}
		}
	}

	/*
	 * Is a shadow volume cached for [key].
	 */
	Bool ForwardRenderManager::HasCachedShadowVolume(const ObjectPairKey& key)  const
	{
		if (shadowVolumeCache.find(key) != shadowVolumeCache.end())
		{
			return true;
		}

		return false;
	}

	/*
	 * Get cached shadow volume for [key].
	 */
	const Point3Array * ForwardRenderManager::GetCachedShadowVolume(const ObjectPairKey& key)
	{
		if (HasCachedShadowVolume(key))
		{
			const Point3Array * shadowVolume = shadowVolumeCache[key];
			return shadowVolume;
		}

		return nullptr;
	}

	/*
	 * Remove and delete all cached shadow volumes.
	 */
	void ForwardRenderManager::DestroyCachedShadowVolumes()
	{
		for (unsigned i = 0; i < shadowVolumeCache.bucket_count(); ++i)
		{
			for (auto iter = shadowVolumeCache.begin(i); iter != shadowVolumeCache.end(i); ++iter)
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
	void ForwardRenderManager::SetForwardBlending(FowardBlendingMethod method)
	{
		forwardBlending = method;
	}

	/*
	 * Get the blending method to be used in forward rendering.
	 */
	FowardBlendingMethod ForwardRenderManager::GetForwardBlending() const
	{
		return forwardBlending;
	}

	/*
	 * Should [camera] render the meshes on [sceneObject]? This is determined by comparing the
	 * layer mask of [sceneObject] with the culling mask of [camera].
	 */
	Bool ForwardRenderManager::ShouldCullFromCamera(const Camera& camera, const SceneObject& sceneObject) const
	{
		// make sure camera's culling mask includes at least one of layers of [sceneObject]
		return !(Engine::Instance()->GetEngineObjectManager()->GetLayerManager().AtLeastOneLayerInCommon(sceneObject.GetLayerMask(), camera.GetCullingMask()));
	}

	/*
	 * Check if [mesh] should be rendered with [light], first based on the culling mask of the light and the layer to
	 * which [sceneObject] belongs, and then based on the distance of the center of [mesh] from [lightPosition].
	 */
	Bool ForwardRenderManager::ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const SceneObject& sceneObject) const
	{
		// exclude objects that have layer masks that are not compatible
		// with the culling mask of the light contained in [lightingDescriptor].
		IntMask layerMask = sceneObject.GetLayerMask();
		IntMask cullingMask = light.GetCullingMask();
		if (!Engine::Instance()->GetEngineObjectManager()->GetLayerManager().AtLeastOneLayerInCommon(layerMask, cullingMask))
		{
			return true;
		}

		if (light.GetType() == LightType::Directional || light.GetType() == LightType::Ambient)
		{
			return false;
		}

		SceneObject& sceneObj = const_cast<SceneObject&>(sceneObject);
		Mesh3DRef meshRef = sceneObj.GetMesh3D();

		if (meshRef.IsValid())
		{
			Mesh3D& mesh = meshRef.GetRef();

			switch (mesh.GetLightCullType())
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
	Bool ForwardRenderManager::ShouldCullBySphereOfInfluence(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const
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
		Real xMag = soiX.QuickMagnitude();
		Real yMag = soiY.QuickMagnitude();
		Real zMag = soiZ.QuickMagnitude();

		// find maximum distance, which will be used as the radius for
		// the sphere of influence
		Real meshMag = xMag;
		if (yMag > meshMag)meshMag = yMag;
		if (zMag > meshMag)meshMag = zMag;

		Vector3 toLight;
		Point3 meshCenter = mesh.GetCenter();
		fullTransform.TransformPoint(meshCenter);

		// get the distance from the light to the mesh's center
		Point3::Subtract(lightPosition, meshCenter, toLight);

		// if the distance from the mesh's center to the light is bigger
		// than the radius of the sphere of influence + the light's range,
		// the the mesh should be culled for the light.
		if (toLight.QuickMagnitude() > meshMag + light.GetRange())return true;

		return false;
	}

	/*
	 * TODO: (Eventually) - Implement tile-base culling.
	 */
	Bool ForwardRenderManager::ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const
	{
		return false;
	}

	/*
	 * Send relevant scene transforms the active shader.
	 * The binding information stored in the active material holds the shader variable locations for these matrices.
	 */
	void ForwardRenderManager::SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& view, const Transform& projection, const Transform& modelViewProjection)
	{
		MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
		ASSERT(activeMaterial.IsValid(), "RenderManager::SendTransformUniformsToShader -> Active material is null.");

		ShaderRef shader = activeMaterial->GetShader();
		ASSERT(shader.IsValid(), "RenderManager::SendTransformUniformsToShader -> Active material contains null shader.");

		Matrix4x4 modelInverseTranspose = model.GetConstMatrix();
		modelInverseTranspose.Transpose();
		modelInverseTranspose.Invert();

		activeMaterial->SendModelMatrixInverseTransposeToShader(&modelInverseTranspose);
		activeMaterial->SendModelMatrixToShader(&model.GetConstMatrix());
		activeMaterial->SendModelViewMatrixToShader(&modelView.GetConstMatrix());
		activeMaterial->SendViewMatrixToShader(&view.GetConstMatrix());
		activeMaterial->SendProjectionMatrixToShader(&projection.GetConstMatrix());
		activeMaterial->SendMVPMatrixToShader(&modelViewProjection.GetConstMatrix());
	}

	/*
	 * Send only the full model-view-projection matrix stored in [modelViewProjection] to the active shader.
	 * The binding information stored in the active material holds the shader variable location for this matrix;
	 */
	void ForwardRenderManager::SendModelViewProjectionToShader(const Transform& modelViewProjection)
	{
		MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
		ASSERT(activeMaterial.IsValid(), "RenderManager::SendModelViewProjectionToShader -> Active material is null.");

		ShaderRef shader = activeMaterial->GetShader();
		ASSERT(shader.IsValid(), "RenderManager::SendModelViewProjectionToShader -> Active material contains null shader.");

		activeMaterial->SendMVPMatrixToShader(&modelViewProjection.GetConstMatrix());
	}

	/*
	 * Send world position of [camera] and all active clips planes for [camera] to the active shader.
	 */
	void ForwardRenderManager::SendCameraAttributesToShader(const Camera& camera, const Point3& cameraPosition)
	{
		MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
		ASSERT(activeMaterial.IsValid(), "RenderManager::SendClipPlaneToShader -> Active material is null.");

		ShaderRef shader = activeMaterial->GetShader();
		ASSERT(shader.IsValid(), "RenderManager::SendClipPlaneToShader -> Active material contains null shader.");

		// for now we only support up to one clip plane
		//TODO: Add support for > 1 clip plane
		if (camera.GetClipPlaneCount() > 0)
		{
			Engine::Instance()->GetGraphicsSystem()->DeactiveAllClipPlanes();
			Engine::Instance()->GetGraphicsSystem()->AddClipPlane();

			const ClipPlane* clipPlane0 = const_cast<Camera&>(camera).GetClipPlane(0);

			if (clipPlane0 != nullptr)
			{
				activeMaterial->SendClipPlaneToShader(0, clipPlane0->Normal.x, clipPlane0->Normal.y, clipPlane0->Normal.z, clipPlane0->Offset);
			}

			activeMaterial->SendClipPlaneCountToShader(1);
		}
		else
		{
			Engine::Instance()->GetGraphicsSystem()->DeactiveAllClipPlanes();
			activeMaterial->SendClipPlaneToShader(0, 0, 0, 0, 0);
			activeMaterial->SendClipPlaneCountToShader(0);
		}

		activeMaterial->SendEyePositionToShader(&cameraPosition);
	}

	/*
	 * Send any custom uniforms specified by the active material to the active shader
	 */
	void ForwardRenderManager::SendActiveMaterialUniformsToShader()  const
	{
		MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
		ASSERT(activeMaterial.IsValid(), "RenderManager::SendCustomUniformsToShader -> Active material is not valid.");
		activeMaterial->SendAllStoredUniformValuesToShader();
	}

	/*
	 * Activate [material], which will switch the GPU's active shader to
	 * the one associated with it.
	 */
	void ForwardRenderManager::ActivateMaterial(MaterialRef material, Bool reverseFaceCulling)
	{
		// We MUST notify the graphics system about the change in active material because other
		// components (like Mesh3DRenderer) need to know about the active material
		Engine::Instance()->GetGraphicsSystem()->ActivateMaterial(material, reverseFaceCulling);
	}
}

