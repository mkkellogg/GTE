#ifndef _VIEWSYSTEM_H_
#define _VIEWSYSTEM_H_

class ViewSystem;

#include "graphics/graphics.h"
#include "global/constants.h"
#include "geometry/transform.h"
#include "camera.h"

class ViewSystem
{
	friend class Graphics;

	const Graphics * graphics;

	Camera * activeCamera;

    protected:

	ViewSystem(const Graphics * graphics);
    ~ViewSystem();

    public:

    Camera * GetActiveCamera();
};

#endif
