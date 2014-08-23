#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

// forward declaration
class Mesh3DRenderer;

#include <vector>

class RenderManager
{
	std::vector<Mesh3DRenderer *> renderers;

    public:

	RenderManager();
    ~RenderManager();
};

#endif
