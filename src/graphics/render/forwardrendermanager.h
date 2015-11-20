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

#include <vector>
#include <map>
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
		 * Data structure that is passed to RenderSceneObjectMeshes() that describes the kind of
		 * lighting to be used (or not used) during rendering.
		 */
		class LightingDescriptor
		{
		public:

			const Light* LightObject;
			const Point3* LightPosition;
			const Vector3* LightDirection;
			Bool UseLighting;

			LightingDescriptor(const Light* lightObject, const Point3* lightPosition, const Vector3 * lightDirection, Bool useLighting) : LightObject(lightObject),
				LightPosition(lightPosition),
				LightDirection(lightDirection)
			{
				this->UseLighting = useLighting;
			
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

				SkyboxObject = nullptr;

				ClipPlaneCount = 0;
				ClipPlane0Offset = 0.0f;
			}
		};

		static const UInt32 MAX_LIGHTS = 16;
		static const UInt32 MAX_CAMERAS = 8;
		static const UInt32 MAX_RENDER_QUEUES = 128;

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
		SceneObject* sceneLights[MAX_LIGHTS];
		// list of ambient lights found in the scene during PreProcessScene()
		SceneObject* sceneAmbientLights[MAX_LIGHTS];
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
		
		void RenderSceneForLight(const Light& light, const ViewDescriptor& viewDescriptor);
		void RenderSceneForLight(const Light& light, const ViewDescriptor& viewDescriptor, Int32 queueID);

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
