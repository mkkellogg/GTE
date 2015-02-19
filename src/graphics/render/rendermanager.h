/*
 * class: RenderManager
 *
 * author: Mark Kellogg
 *
 * The RenderManager is responsible for processing the scene graph
 * and rendering all objects with meshes and mesh renderers attached
 * to them.
 */

#ifndef _GTE_RENDER_MANAGER_H
#define _GTE_RENDER_MANAGER_H

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

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include "object/engineobject.h"
#include "util/datastack.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "geometry/transform.h"

class RenderManager
{
	/*
	 * Data structure that is passed to RenderSceneObjectMeshes() that describes the kind of
	 * lighting to be used (or not used) during rendering.
	 */
	class LightingDescriptor
	{
		public:

		const Light* LightObject;
		const Point3* LightPosition;
		bool SelfLit;

		LightingDescriptor(const Light* lightObject, const Point3* lightPosition, bool selfLit) : LightObject(lightObject), LightPosition(lightPosition)
		{
			this->SelfLit = selfLit;
		}
	};

	/*
	 * This class is used as the key in the renderedObjects hashing structure
	 */
	class ObjectPairKey
	{
		public:

		ObjectID ObjectAID;
		ObjectID ObjectBID;

		ObjectPairKey()
		{
			ObjectAID = 0;
			ObjectBID = 0;
		}

		ObjectPairKey(ObjectID objectAID, ObjectID objectBID)
		{
			ObjectAID = objectAID;
			ObjectBID = objectBID;
		}

		 // TODO: optimize this hashing function (implement correctly)
		typedef struct
		{
			 int operator()(const ObjectPairKey& s) const
			 {
				  return ((int)s.ObjectAID << 1) +  ((int)s.ObjectBID << 2);
			 }
		}ObjectPairKeyHasher;

		typedef struct
		{
		  bool operator() (const ObjectPairKey& a, const ObjectPairKey& b) const { return a==b; }
		} ObjectPairKeyEq;

		bool operator==(const ObjectPairKey& s) const
		{
			return s.ObjectAID == this->ObjectAID && s.ObjectBID == this->ObjectBID;
		}
	};

	static const int MAX_LIGHTS = 16;
	static const int MAX_CAMERAS = 8;
	static const int MAX_SCENE_MESHES = 128;

	// material for rendering shadow volumes
	MaterialRef shadowVolumeMaterial;

	// material for rendering only to the depth buffer
	MaterialRef depthOnlyMaterial;

	// transform stack used for processing scene hierarchy
	DataStack<Matrix4x4> sceneProcessingStack;

	// number of meshes found in the scene during ProcessScene()
	unsigned int sceneMeshCount;
	// number of lights found in the scene during ProcessScene()
	unsigned int lightCount;
	// number of ambient lights found in the scene during  ProcessScene()
	unsigned int ambientLightCount;
	// number of cameras found in the scene during  ProcessScene()
	unsigned int cameraCount;

	// list of meshes found in the scene during ProcessScene()
	SceneObjectRef sceneMeshObjects[MAX_SCENE_MESHES];
	// list of lights found in the scene during ProcessScene()
	SceneObjectRef sceneLights[MAX_LIGHTS];
	// list of ambient lights found in the scene during ProcessScene()
	SceneObjectRef sceneAmbientLights[MAX_LIGHTS];
	// list of cameras found in the scene during ProcessScene()
	SceneObjectRef sceneCameras[MAX_CAMERAS];

	// keep track of objects that have been rendered
	// TODO: optimize usage of this hashing structure
	std::unordered_map<ObjectPairKey, bool, ObjectPairKey::ObjectPairKeyHasher,ObjectPairKey::ObjectPairKeyEq> renderedObjects;

	std::unordered_map<ObjectPairKey, Point3Array*, ObjectPairKey::ObjectPairKeyHasher,ObjectPairKey::ObjectPairKeyEq> shadowVolumeCache;

	void ProcessScene();
	void ProcessScene(SceneObject& parent, Transform& aggregateTransform);
	void RenderSceneForCamera(unsigned int cameraIndex);
	void ForwardRenderSceneForCamera(Camera& camera);
	void RenderSkyboxForCamera(Camera& camera, const Transform& viewTransformInverse);
	void FillDepthBuffer(const Transform& viewTransformInverse, const Camera& camera);
	void ForwardRenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& viewTransformInverse, const Camera& camera);
	void ForwardRenderSceneForSelfLit(const Transform& viewTransformInverse, const Camera& camera);
	void ForwardRenderSceneObjectMeshes(SceneObject& sceneObject, const LightingDescriptor& lightingDescriptor, const Transform& viewTransformInverse, const Camera& camera,
								 MaterialRef materialOverride, bool flagRendered);
	void RenderShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition,  const Transform& viewTransformInverse, const Camera& camera);
	bool ValidateSceneObjectForRendering(SceneObjectRef sceneObject);
	bool HasSceneObjectBeenRendered(SceneObjectRef sceneObject);
	void BuildShadowVolumeMVPTransform(const Light& light, const Point3& meshCenter, const Transform& modelTransform, const Point3& modelLocalLightPos, const Vector3& modelLocalLightDir,
			 	 	 	 	 	 	   const Camera& camera, const Transform& viewTransformInverse, Transform& outTransform, float xScale, float yScale);
    void CacheShadowVolume(ObjectPairKey& key, const Point3Array * positions);
    void ClearCachedShadowVolume(ObjectPairKey& key);
    bool HasCachedShadowVolume(ObjectPairKey& key);
    Point3Array * GetCachedShadowVolume(ObjectPairKey& key);
    void DestroyCachedShadowVolumes();

    void ClearBuffersForCamera(const Camera& camera) const;
    void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
    void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);
    unsigned int RenderDepth(const DataStack<Matrix4x4>& transformStack) const;
    void ActivateMaterial(MaterialRef material);
    void SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& projection, const Transform& modelViewProjection);
    void SendModelViewProjectionToShader(const Transform& modelViewProjection);
    void SendActiveMaterialUniformsToShader();

    bool ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;
    bool ShouldCullBySphereOfInfluence(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;
    bool ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;

    public:

	RenderManager();
    ~RenderManager();

    bool Init();

    void RenderAll();
};

#endif
