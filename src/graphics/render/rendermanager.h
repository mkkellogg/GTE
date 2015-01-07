/*
 * class: RenderManager
 *
 * author: Mark Kellogg
 *
 * The RenderManager is responsible for processing the scene graph
 * and rendering all objects with meshes and mesh renderers attached
 * to them.
 */

#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

// forward declaration
class SceneObject;
class Graphics;
class EngineObjectManager;
class Transform;
class Material;
class SceneObjectComponent;
class SubMesh3D;
class Transform;

#include <vector>
#include <map>
#include <memory>
#include "object/engineobject.h"
#include "util/datastack.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "geometry/transform.h"

class RenderManager
{
	static const int MAX_LIGHTS = 16;
	static const int MAX_CAMERAS = 8;
	static const int MAX_SCENE_MESHES = 128;

	class RenderAffector
	{
		public:

		Transform AffectorTransform;
		Camera * AffectorCamera = NULL;
		Light * AffectorLight = NULL;
	};

	unsigned int sceneMeshCount;
	SceneObjectRef sceneMeshObjects[MAX_SCENE_MESHES];

	int cycleCount;

	Graphics * graphics;
	EngineObjectManager * objectManager;

	MaterialRef shadowVolumeMaterial;

	DataStack<Matrix4x4> viewTransformStack;
	DataStack<Matrix4x4> modelTransformStack;

	unsigned int lightCount;
	unsigned int ambientLightCount;
	RenderAffector sceneLights[MAX_LIGHTS];
	RenderAffector sceneAmbientLights[MAX_LIGHTS];

	unsigned int cameraCount;
	RenderAffector sceneCameras[MAX_CAMERAS];

	std::map<ObjectID, bool> renderedObjects;

	void ProcessScene();
	void ProcessScene(SceneObject& parent, Transform& aggregateTransform);
	void RenderSceneFromCamera(unsigned int cameraIndex);
	void ForwardRenderScene(const Transform& viewTransformInverse, const Camera& camera);
	void RenderSceneForLight(const Light& light, const Transform& lightFullTransform, const Transform& viewTransformInverse, const Camera& camera, bool depthBufferComplete);
	void RenderSceneObjectMeshes(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Transform& viewTransformInverse, const Camera& camera);
	void RenderShadowVolumesForSceneObject(SceneObject& sceneObject, const Light& light, const Point3& lightPosition, const Transform& lightTransform, const Transform& lightTransformInverse,
											  const Transform& viewTransformInverse, const Camera& camera);
	void BuildShadowVolumeMVPTransform(const Light& light, const Point3& meshCenter, const Transform& modelTransform, const Point3& modelLocalLightPos, const Vector3& modelLocalLightDir,
			 	 	 	 	 	 	   const Camera& camera, const Transform& viewTransformInverse, Transform& outTransform);

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
