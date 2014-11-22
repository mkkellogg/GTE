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
	class RenderSceneObjectComponent
	{
		public:

		Transform transform;
		SceneObjectComponent * component = NULL;
	};

	int cycleCount;

	Graphics * graphics;
	EngineObjectManager * objectManager;

	DataStack<Matrix4x4> viewTransformStack;
	DataStack<Matrix4x4> modelTransformStack;

	MaterialRef activeMaterial;

	static const int MAX_LIGHTS = 16;
	unsigned int lightCount;
	RenderSceneObjectComponent sceneLights[MAX_LIGHTS];

	static const int MAX_CAMERAS = 8;
	unsigned int cameraCount;
	RenderSceneObjectComponent sceneCameras[MAX_CAMERAS];

	std::map<ObjectID, bool> renderedObjects;

	void ForwardRenderScene(SceneObject& parent, Transform& viewTransformInverse, Camera& camera);
    void ClearBuffersForCamera(const Camera& camera) const;
    void PushTransformData(const Transform& transform, DataStack<Matrix4x4>& transformStack);
    void PopTransformData(Transform& transform, DataStack<Matrix4x4>& transformStack);
    unsigned int RenderDepth(const DataStack<Matrix4x4>& transformStack) const;
    void ActivateMaterial(MaterialRef material);
    void SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& projection);
    void SendCustomUniformsToShader();

    void ProcessScene();
    void ProcessScene(SceneObject * parent, Transform& aggregateTransform);
    void RenderSceneFromCamera(unsigned int cameraIndex);

    bool ShouldCullFromLight(Light& light, Point3& lightPosition, Transform& fullTransform, Mesh3D& mesh) const;
    bool ShouldCullBySphereOfInfluence(Light& light, Point3& lightPosition, Transform& fullTransform,  Mesh3D& mesh) const;
    bool ShouldCullByTile(Light& light, Point3& lightPosition, Transform& fullTransform,  Mesh3D& mesh) const;

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    bool Init();

    void RenderAll();
};

#endif
