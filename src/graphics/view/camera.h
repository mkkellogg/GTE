#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera;

#include "graphics/graphics.h"
#include "graphics/renderbuffer.h"
#include "global/constants.h"
#include "geometry/transform.h"

class Camera
{
	friend class ViewSystem;

    Transform * modelViewTransform;
    Transform * projectionTransform;
    Transform * mvpTransform;

    unsigned int clearBufferMask;

    protected:

    Camera();
    ~Camera();

    public:

    const Transform * GetModelViewTransform() const ;
    const Transform * GetProjectionTransform() const ;
    const Transform * GetMVPTransform() const ;
    void AddClearBuffer(RenderBufferType buffer);
    void RemoveClearBuffer(RenderBufferType buffer);
    unsigned int GetClearBufferMask() const;
};

#endif
