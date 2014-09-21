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

#include <vector>
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

	Graphics * graphics;
	EngineObjectManager * objectManager;
	DataStack<float> * viewTransformStack;
	DataStack<float> * modelTransformStack;
	Material * activeMaterial;

	static const int MAX_LIGHTS = 16;
	unsigned int lightCount;
	RenderSceneObjectComponent sceneLights[MAX_LIGHTS];

	static const int MAX_CAMERAS = 8;
	unsigned int cameraCount;
	RenderSceneObjectComponent sceneCameras[MAX_CAMERAS];

	void RenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera);
    void ClearBuffersForCamera(const Camera * camera) const;
    void PushTransformData(const Transform * transform, DataStack<float> * transformStack);
    void PopTransformData(const Transform * transform, DataStack<float> * transformStack);
    int RenderDepth(const DataStack<float> * transformStack) const;
    void ActivateMaterial(Material * material);
    void SendTransformUniformsToShader(const Transform * model, const Transform * modelView, const Transform * projection);
    void SendCustomUniformsToShader();

    void ProcessScene(SceneObject * parent, Transform * viewTransform);
    void RenderSceneFromCamera(unsigned int cameraIndex);

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    bool Init();

    void RenderAll();
};

#endif
