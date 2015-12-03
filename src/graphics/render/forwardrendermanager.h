/*
 * class: ForwardRenderManager
 *
 * author: Mark Kellogg
 *
 * This class is responsible for rendering the scene using forward 
 * rendering.
 
 * It processes the scene graph and builds data structures describing all meshes that
 * need to be rendered as well as the lights and cameras for which they will
 * be rendered.
 */

#ifndef _GTE_FORWARD_RENDER_MANAGER_H
#define _GTE_FORWARD_RENDER_MANAGER_H

#include "engine.h"
#include "rendermanager.h"
#include "object/engineobject.h"
#include "object/objectpairkey.h"
#include "util/datastack.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "geometry/transform.h"
#include "renderqueue.h"
#include "renderqueuemanager.h"
#include "assert.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <stack>

namespace GTE
{
	// forward declaration
	class SceneObject;
	class Graphics;
	class EngineObjectManager;
	class Transform;
	class Material;
	class SceneObjectComponent;
	class SubMesh3D;
	class Transform;
	class Point3Array;

	enum class FowardBlendingMethod
	{
		Additive = 0,
		Subtractive = 1
	};

	enum class FowardBlendingFilter
	{
		Never = 0,
		Always = 1,
		OnlyIfRendered = 2
	};

	class ForwardRenderManager : public RenderManager
	{
		friend class Engine;

		/*
		 * Data structure that is passed to RenderSceneObjectMeshes() that describes lighting
		 * information to be used (or not used) during rendering.
		 */
		class LightingDescriptor
		{
			Bool initialized;

		public:

			UInt32 LightCount;
			Light** LightObjects;

			Real* PositionDatas;
			Real* DirectionDatas;
			Real* ColorDatas;
			Point3 * Positions;
			Vector3 * Directions;
			Color4 * Colors;
			Int32* Types;
			Real* Intensities;
			Real* Ranges;
			Real* Attenuations;
			Int32* ParallelAngleAttenuations;
			Int32* OrthoAngleAttenuations;

			Bool UseLighting;

			LightingDescriptor()
			{
				LightObjects = nullptr;

				Positions = nullptr;
				Directions = nullptr;
				Colors = nullptr;

				PositionDatas = nullptr;
				DirectionDatas = nullptr;
				ColorDatas = nullptr;

				Types = nullptr;
				Intensities = nullptr;
				Ranges = nullptr;
				Attenuations = nullptr;
				ParallelAngleAttenuations = nullptr;
				OrthoAngleAttenuations = nullptr;

				initialized = false;
				UseLighting = true;
				LightCount = 0;
			}
			
			~LightingDescriptor()
			{
				SAFE_DELETE(LightObjects);

				if(Positions != nullptr)
				{
					delete[] Positions;
					Positions = nullptr;
				}

				if(Directions != nullptr)
				{
					delete[] Directions;
					Directions = nullptr;
				}

				if(Colors != nullptr)
				{
					delete[] Colors;
					Colors = nullptr;
				}

				SAFE_DELETE(PositionDatas);
				SAFE_DELETE(DirectionDatas);
				SAFE_DELETE(ColorDatas);

				SAFE_DELETE(Types);
				SAFE_DELETE(Intensities);
				SAFE_DELETE(Ranges);
				SAFE_DELETE(Attenuations);
				SAFE_DELETE(ParallelAngleAttenuations);
				SAFE_DELETE(OrthoAngleAttenuations);
			}

			Bool Init(UInt32 maxLights)
			{
				if(!initialized)
				{
					LightObjects = new Light*[maxLights];
					ASSERT(LightObjects != nullptr, "LightingDescriptor::Init -> Unable to allocate light objects.");

					Positions = new Point3[maxLights];
					ASSERT(Positions != nullptr, "LightingDescriptor::Init -> Unable to allocate light positions.");

					Directions = new Vector3[maxLights];
					ASSERT(Directions != nullptr, "LightingDescriptor::Init -> Unable to allocate light directions.");

					Colors = new Color4[maxLights];
					ASSERT(Colors != nullptr, "LightingDescriptor::Init -> Unable to allocate light colors.");

					PositionDatas = new Real[maxLights * 4];
					ASSERT(PositionDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light position data.");

					DirectionDatas = new Real[maxLights * 4];
					ASSERT(DirectionDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light direction data.");

					ColorDatas = new Real[maxLights * 4];
					ASSERT(ColorDatas != nullptr, "LightingDescriptor::Init -> Unable to allocate light color data.");

					Types = new Int32[maxLights];
					ASSERT(Types != nullptr, "LightingDescriptor::Init -> Unable to allocate light types.");

					Intensities = new Real[maxLights];
					ASSERT(Intensities != nullptr, "LightingDescriptor::Init -> Unable to allocate light intensities.");

					Ranges = new Real[maxLights];
					ASSERT(Ranges != nullptr, "LightingDescriptor::Init -> Unable to allocate light ranges.");

					Attenuations = new Real[maxLights];
					ASSERT(Attenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light attenuations.");

					OrthoAngleAttenuations = new Int32[maxLights];
					ASSERT(OrthoAngleAttenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light ortho-angle attenuations.");

					ParallelAngleAttenuations = new Int32[maxLights];
					ASSERT(ParallelAngleAttenuations != nullptr, "LightingDescriptor::Init -> Unable to allocate light parallel angle attenuations.");

					initialized = true;
				}

				return true;
			}
		};

		/*
		* Data structure that is passed to RenderSceneObjectMeshes() that describes properties
		* such as the view transform, inverse view transform, and view position.
		*/
		class ViewDescriptor
		{
			public:

			IntMask ClearBufferMask;
			IntMask CullingMask;

			Bool ReverseCulling;

			Transform ViewTransform;
			Transform ViewTransformInverse;

			Point3 ViewPosition;

			Transform ProjectionTransform;
			Transform ProjectionTransformInverse;
			Transform UniformWorldSceneObjectTransform;
			Transform UniformWorldSceneObjectTransformInverse;

			Bool AmbientPassEnabled;
			Bool DepthPassEnabled;

			Bool SSAOEnabled;
			SSAORenderMode SSAOMode;

			Bool SkyboxEnabled;
			SceneObject * SkyboxObject;

			UInt32 ClipPlaneCount;
			Vector3 ClipPlane0Normal;
			Real ClipPlane0Offset;

			ViewDescriptor()
			{
				CullingMask = 0;
				ClearBufferMask = 0;

				ReverseCulling = false;

				AmbientPassEnabled = true;
				DepthPassEnabled = true;

				SSAOEnabled = true;
				SSAOMode = SSAORenderMode::Standard;

				SkyboxEnabled = false;
				SkyboxObject = nullptr;

				ClipPlaneCount = 0;
				ClipPlane0Offset = 0.0f;
			}
		};

		static const UInt32 MAX_CAMERAS = 8;
		static const UInt32 MAX_RENDER_QUEUES = 128;

		// describes parameters of a single light
		LightingDescriptor singleLightDescriptor;
		// describes parameters of a set of lights
		LightingDescriptor multiLightDescriptor;

		RenderQueueEntry skyboxEntry;
		// mesh for doing full screen effects
		Mesh3DSharedPtr fullScreenQuad;
		// Ortho camera for rendering to [fullScreenQuad]
		CameraSharedPtr fullScreenQuadCam;
		// scene object for full screen quad mesh
		SceneObjectSharedPtr fullScreenQuadObject;
		// material for rendering shadow volumes
		MaterialSharedPtr shadowVolumeMaterial;
		// material for rendering only to the depth buffer
		MaterialSharedPtr depthOnlyMaterial;
		// material for rendering depth values to color buffer
		MaterialSharedPtr depthValueMaterial;
		// material for rendering SSAO-style outlines
		MaterialSharedPtr ssaoOutlineMaterial;
		// for off-screen rendering
		RenderTargetSharedPtr depthRenderTarget;

		// number of renderable scene objects found during scene processing
		UInt32 renderableSceneObjectCount;
		// number of lights found in the scene during PreProcessScene()
		UInt32 lightCount;
		// number of ambient lights found in the scene during  PreProcessScene()
		UInt32 ambientLightCount;
		// number of cameras found in the scene during  PreProcessScene()
		UInt32 cameraCount;

		// render queue manager
		RenderQueueManager renderQueueManager;
		// scene objects that contain valid renderables
		SceneObject* renderableSceneObjects[Constants::MaxSceneObjects];
		// list of lights found in the scene during PreProcessScene()
		SceneObject* sceneLights[Constants::MaxSceneLights];
		// list of ambient lights found in the scene during PreProcessScene()
		SceneObject* sceneAmbientLights[Constants::MaxSceneLights];
		// list of cameras found in the scene during PreProcessScene()
		SceneObject* sceneCameras[MAX_CAMERAS];

		// current blending method used in forward rendering
		FowardBlendingMethod forwardBlending;

		// keep track of objects that have been rendered
		// TODO: optimize usage of this hashing structure
		std::unordered_map<UInt32, Bool> renderedObjects;
		// cache shadow volumes that don't need to be constantly rebuilt
		std::unordered_map<ObjectPairKey, Point3Array*, ObjectPairKey::ObjectPairKeyHasher, ObjectPairKey::ObjectPairKeyEq> shadowVolumeCache;

		std::stack<const RenderTargetSharedPtr*> renderTargetStack;

		void PreRender() override;
		void PreProcessScene(SceneObject& parent, UInt32 recursionDepth);
		void PreRenderScene();

		void RenderSceneForCamera(UInt32 cameraIndex);
		void RenderSceneForCamera(CameraRef camera);

		void GetViewDescriptorForCamera(const Camera& camera, const Transform* altViewTransform, ViewDescriptor& descriptor);
		void ClearRenderedStatus();

		void RenderSceneForCurrentRenderTarget(const ViewDescriptor& viewDescriptor);
		void RenderSkyboxForCamera(const ViewDescriptor& viewDescriptor);
		void RenderDepthBuffer(const ViewDescriptor& viewDescriptor);
		void RenderSceneSSAO(const ViewDescriptor& viewDescriptor);

		void RenderSceneForLight(const Light& light, const ViewDescriptor& viewDescriptor, Int32 queueID);
		void RenderSceneForMultiLight(const ViewDescriptor& viewDescriptor, Int32 queueID);

		void RenderSceneWithoutLight(const ViewDescriptor& viewDescriptor, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce, 
									 FowardBlendingFilter blendingFilter,  std::function<Bool(SceneObject*)> filterFunction);
		void RenderSceneWithoutLight(const ViewDescriptor& viewDescriptor, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce,
									 FowardBlendingFilter blendingFilter, std::function<Bool(SceneObject*)> filterFunction, Int32 queueID);

		void RenderMesh(RenderQueueEntry& entry, const LightingDescriptor& lightingDescriptor, const ViewDescriptor& viewDescriptor, 
						MaterialRef materialOverride, Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter);

		void RenderShadowVolumeForMesh(RenderQueueEntry& entry, const Light& light, const Point3& lightPosition, const Vector3& lightDirection,
									   const ViewDescriptor& viewDescriptor);
		
		void BuildShadowVolumeMVPTransform(const Transform& modelTransform, const Transform& viewTransformInverse, const Transform& projectionTransform,
										   Transform& outTransform, Real xScale, Real yScale) const;
		void BuildSceneShadowVolumes();
		void BuildShadowVolumesForLight(const Light& light, const Transform& lightWorldTransform);
		void BuildShadowVolumesForMesh(RenderQueueEntry& entry, const Light& light, const Point3& lightPosition, const Vector3& lightDirection);
		void CacheShadowVolume(const ObjectPairKey& key, const Point3Array * positions);
		void ClearCachedShadowVolume(const ObjectPairKey& key);
		Bool HasCachedShadowVolume(const ObjectPairKey& key)  const;
		const Point3Array * GetCachedShadowVolume(const ObjectPairKey& key);
		void DestroyCachedShadowVolumes();

		void SetForwardBlending(FowardBlendingMethod method);
		FowardBlendingMethod GetForwardBlending() const;

		void ClearRenderBuffers(IntMask clearMask) const;

		void ActivateMaterial(MaterialRef material, Bool reverseFaceCulling);
		void SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& view, const Transform& projection, const Transform& modelViewProjection);
		void SendModelViewProjectionToShader(const Transform& modelViewProjection);
		void SendViewAttributesToShader(const ViewDescriptor& viewDescriptor);
		void SendActiveMaterialUniformsToShader() const;

		Bool ShouldCullByLayer(IntMask cullingMask, const SceneObject& sceneObject) const;
		Bool ShouldCullFromLightByPosition(const Light& light, const Point3& lightWorldPosition, const SubMesh3D& mesh, const Transform& meshWorldTransformInverse) const;
		Bool ShouldCullFromLightByLayer(const Light& light, const SceneObject& sceneObject) const;
		Bool ShouldCullByBoundingBox(const Light& light, const Point3& lightPosition, const Transform& meshWorldTransform, const SubMesh3D& mesh) const;
		Bool ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& meshWorldTransform, const SubMesh3D& mesh) const;

		void PushRenderTarget(RenderTargetRef renderTarget);
		RenderTargetRef PopRenderTarget();
		void ClearRenderTargetStack();

		Bool InitFullScreenQuad();

	public:

		ForwardRenderManager();
		~ForwardRenderManager();

		Bool Init();
		void RenderScene() override;
		void ClearCaches() override;

		void RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers) override;
	};
}

#endif
