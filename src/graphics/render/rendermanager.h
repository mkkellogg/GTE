#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

// forward declaration
class SceneObject;
class Camera;
class Graphics;

#include <vector>

class RenderManager
{
	std::vector<SceneObject *> renderers;
	Graphics * graphics;

    public:

	RenderManager(Graphics * graphics);
    ~RenderManager();

    void AddMeshRenderer(SceneObject * renderer);
    void RemoveMeshRenderer(SceneObject * renderer);
    void RenderAll(const Camera * camera);
};

#endif
