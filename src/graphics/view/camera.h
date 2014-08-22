#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera;

#include "graphics/graphics.h"
#include "global/constants.h"
#include "geometry/transform.h"

class Camera
{
	friend class ViewSystem;

	const ViewSystem * viewSystem;

    Transform * modelViewTransform;
    Transform * projectionTransform;
    Transform * mvpTransform;

    protected:

    Camera(const ViewSystem * viewSystem);
    ~Camera();

    public:

    const Transform * GetModelViewTransform() const ;
    const Transform * GetProjectionTransform() const ;
    const Transform * GetMVPTransform() const ;
};

#endif
