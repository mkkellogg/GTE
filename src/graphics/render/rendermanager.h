#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

// forward declaration
class SceneObject;
class Camera;
class Graphics;
class EngineObjectManager;
class Transform;

#include <vector>

class RenderManager
{
	Graphics * graphics;
	EngineObjectManager * objectManager;

	void RenderScene(Transform *modelViewTransform, Camera * camera);

    public:

	RenderManager(Graphics * graphics, EngineObjectManager * objectManager);
    ~RenderManager();

    void RenderAll();
};

#endif
