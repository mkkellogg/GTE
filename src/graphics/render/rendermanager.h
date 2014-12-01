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
	SceneObject * meshObjects[MAX_SCENE_MESHES];

	int cycleCount;

	Graphics * graphics;
	EngineObjectManager * objectManager;

	DataStack<Matrix4x4> viewTransformStack;
	DataStack<Matrix4x4> modelTransformStack;

	MaterialRef activeMaterial;

	unsigned int lightCount;
	RenderAffector sceneLights[MAX_LIGHTS];

	unsigned int cameraCount;
	RenderAffector sceneCameras[MAX_CAMERAS];

	std::map<ObjectID, bool> renderedObjects;

	void ProcessScene();
	void ProcessScene(SceneObject * parent, Transform& aggregateTransform);
	void RenderSceneFromCamera(unsigned int cameraIndex);
	void ForwardRenderScene(const Transform& viewTransformInverse, const Camera& camera);
	void RenderSceneForLight(const Light& light, const Point3& lightPosition, const Transform& viewTransformInverse, const Camera& camera);
    void ClearBuffersForCamera(const Camera& camera) const;
    void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
    void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);
    unsigned int RenderDepth(const DataStack<Matrix4x4>& transformStack) const;
    void ActivateMaterial(MaterialRef material);
    void SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& projection);
    void SendCustomUniformsToShader();


    bool ShouldCullFromLight(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;
    bool ShouldCullBySphereOfInfluence(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;
    bool ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& fullTransform, const Mesh3D& mesh) const;

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    bool Init();

    void RenderAll();
};

#endif
