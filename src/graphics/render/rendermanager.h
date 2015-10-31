/*
 * class: RenderManager
 *
 * author: Mark Kellogg
 *
 * The RenderManager is responsible for rendering the scene. It processes
 * the scene graph and builds data structures describing all meshes that
 * need to be rendered as well as the lights and cameras for which they will
 * be rendered.
 */

#ifndef _GTE_RENDER_MANAGER_H
#define _GTE_RENDER_MANAGER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <stack>
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "object/objectpairkey.h"
#include "util/datastack.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "geometry/transform.h"

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

	class RenderManager
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

		static const Int32 MAX_LIGHTS = 16;
		static const Int32 MAX_CAMERAS = 8;
		static const Int32 MAX_SCENE_MESHES = 128;

		// mesh for doing full screen effects
		Mesh3DRef fullScreenQuad;
		// Ortho camera for rendering to [fullScreenQuad]
		CameraRef fullScreenQuadCam;
		// scene object for full screen quad mesh
		SceneObjectRef fullScreenQuadObject;
		// material for rendering shadow volumes
		MaterialRef shadowVolumeMaterial;
		// material for rendering only to the depth buffer
		MaterialRef depthOnlyMaterial;
		// material for rendering depth values to color buffer
		MaterialRef depthValueMaterial;
		// material for rendering SSAO-style outlines
		MaterialRef ssaoOutlineMaterial;
		// for off-screen rendering
		RenderTargetRef depthRenderTarget;
		// transform stack used for processing scene hierarchy
		DataStack<Matrix4x4> sceneProcessingStack;

		// number of meshes found in the scene during ProcessScene()
		UInt32 sceneMeshCount;
		// number of lights found in the scene during ProcessScene()
		UInt32 lightCount;
		// number of ambient lights found in the scene during  ProcessScene()
		UInt32 ambientLightCount;
		// number of cameras found in the scene during  ProcessScene()
		UInt32 cameraCount;

		// list of meshes found in the scene during ProcessScene()
		SceneObjectRef sceneMeshObjects[MAX_SCENE_MESHES];
		// list of lights found in the scene during ProcessScene()
		SceneObjectRef sceneLights[MAX_LIGHTS];
		// list of ambient lights found in the scene during ProcessScene()
		SceneObjectRef sceneAmbientLights[MAX_LIGHTS];
		// list of cameras found in the scene during ProcessScene()
		SceneObjectRef sceneCameras[MAX_CAMERAS];
		// current blending method used in forward rendering
		FowardBlendingMethod forwardBlending;

		// keep track of objects that have been rendered
		// TODO: optimize usage of this hashing structure
		std::unordered_map<ObjectPairKey, Bool, ObjectPairKey::ObjectPairKeyHasher, ObjectPairKey::ObjectPairKeyEq> renderedObjects;
		// cache shadow volumes that don't need to be constantly rebuilt
		std::unordered_map<ObjectPairKey, Point3Array*, ObjectPairKey::ObjectPairKeyHasher, ObjectPairKey::ObjectPairKeyEq> shadowVolumeCache;

		std::stack<RenderTargetRef> renderTargetStack;

		void PreProcessScene();
		void PreProcessScene(SceneObject& parent, Transform& aggregateTransform);
		void PreRenderScene();

		void RenderSceneForCamera(UInt32 cameraIndex);
		void ForwardRenderSceneForCamera(Camera& camera);
		void ForwardRenderSceneForCameraAndCurrentRenderTarget(Camera& camera, const Transform& viewTransform, const Transform& viewInverse);
		void ForwardRenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& modelPreTransform,
			const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera);
		void ForwardRenderSceneForSelfLitMaterials(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera);
		void ForwardRenderSceneObject(SceneObject& sceneObject, const LightingDescriptor& lightingDescriptor, const Transform& modelPreTransform,
			const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera, MaterialRef materialOverride,
			Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter);
		void ForwardRenderSkyboxForCamera(Camera& camera, const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse);
		void ForwardRenderDepthBuffer(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera);
		void ForwardRenderSceneSSAO(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera);
		void ForwardRenderSceneWithSelfLitLighting(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera,
			MaterialRef material, Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter);
		void ForwardRenderSceneWithSelfLitLighting(const Transform& modelPreTransform, const Transform& viewTransform, const Transform& viewTransformInverse, const Camera& camera,
			MaterialRef material, Bool flagRendered, Bool renderMoreThanOnce, FowardBlendingFilter blendingFilter,
			std::function<Bool(SceneObjectRef)> filterFunction);
		Bool ValidateSceneObjectForRendering(SceneObjectRef sceneObject) const;
		void BuildShadowVolumeMVPTransform(const Transform& modelTransform,  const Camera& camera, const Transform& viewTransformInverse, Transform& outTransform, Real xScale, Real yScale) const;
		void RenderShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Vector3& lightDirection,
			const Transform& modelPreTransform, const Transform& viewTransformInverse, const Camera& camera);
		void BuildSceneShadowVolumes();
		void BuildShadowVolumesForLight(const Light& light, const Transform& lightFullTransform);
		void BuildShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Vector3& lightDirection);
		void CacheShadowVolume(const ObjectPairKey& key, const Point3Array * positions);
		void ClearCachedShadowVolume(const ObjectPairKey& key);
		Bool HasCachedShadowVolume(const ObjectPairKey& key)  const;
		const Point3Array * GetCachedShadowVolume(const ObjectPairKey& key);
		void DestroyCachedShadowVolumes();

		void SetForwardBlending(FowardBlendingMethod method);
		FowardBlendingMethod GetForwardBlending() const;

		void ClearBuffersForCamera(const Camera& camera) const;

		void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
		void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);
		UInt32 RenderDepth(const DataStack<Matrix4x4>& transformStack) const;

		void ActivateMaterial(MaterialRef material, Bool reverseFaceCulling);
		void SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& view, const Transform& projection, const Transform& modelViewProjection);
		void SendModelViewProjectionToShader(const Transform& modelViewProjection);
		void SendCameraAttributesToShader(const Camera& camera, const Point3& cameraPosition);
		void SendActiveMaterialUniformsToShader() const;

		Bool ShouldCullFromCamera(const Camera& camera, const SceneObject& sceneObject) const;
		Bool ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const SceneObject& sceneObject) const;
		Bool ShouldCullBySphereOfInfluence(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;
		Bool ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;

		void PushRenderTarget(RenderTargetRef renderTarget);
		RenderTargetRef PopRenderTarget();
		void ClearRenderTargetStack();

		Bool InitFullScreenQuad();

	public:

		RenderManager();
		~RenderManager();

		Bool Init();
		void RenderScene();
		void ClearCaches();

		void RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers);
	};
}

#endif
