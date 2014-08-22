#ifndef _VIEWSYSTEM_H_
#define _VIEWSYSTEM_H_

class ViewSystem;

#include "graphics/graphics.h"
#include "global/constants.h"
#include "geometry/transform.h"
#include "camera.h"
#include <vector>

class ViewSystem
{
	friend class Graphics;

	const Graphics * graphics;

	std::vector<Camera*> cameras;

	void DestroyCameras();
	void DestroyCamera(Camera * camera);

    protected:

	ViewSystem(const Graphics * graphics);
    ~ViewSystem();

    public:

    int CameraCount() const;
    const Camera * GetCamera(int index) const;
};

#endif
