#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

// forward declaration
class SceneObject;
class Camera;
class Graphics;
class EngineObjectManager;
class Transform;

#include <vector>
#include "util/datastack.h"

class RenderManager
{
	Graphics * graphics;
	EngineObjectManager * objectManager;
	DataStack<float> * viewTransformStack;
	DataStack<float> * modelTransformStack;

	void RenderScene(SceneObject * parent, Transform * modelTransform, Transform * viewTransformInverse, Camera * camera);
    void ClearBuffersForCamera(const Camera * camera) const;
    void PushTransformData(const Transform * transform, DataStack<float> * transformStack);
    void PopTransformData(Transform * transform, DataStack<float> * transformStack);
    int RenderDepth(DataStack<float> * transformStack);

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    void RenderAll();
    void RenderAll(SceneObject * parent, Transform * viewTransform);
};

#endif
