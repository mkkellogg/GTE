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
class Camera;
class Graphics;
class EngineObjectManager;
class Transform;
class Material;

#include <vector>
#include "util/datastack.h"

class RenderManager
{
	Graphics * graphics;
	EngineObjectManager * objectManager;
	DataStack<float> * viewTransformStack;
	DataStack<float> * modelTransformStack;
	Material * activeMaterial;

	void RenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera);
    void ClearBuffersForCamera(const Camera * camera) const;
    void PushTransformData(const Transform * transform, DataStack<float> * transformStack);
    void PopTransformData(const Transform * transform, DataStack<float> * transformStack);
    int RenderDepth(const DataStack<float> * transformStack) const;
    void ActivateMaterial(Material * material);
    void SendTransformUniformsToShader(const Transform * modelView, const Transform * projection);
    void SendCustomUniformsToShader();

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    void RenderAll();
    void RenderFromCameras(SceneObject * parent, Transform * viewTransform);

};

#endif
