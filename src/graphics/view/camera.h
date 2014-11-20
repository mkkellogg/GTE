#ifndef _CAMERA_H_
#define _CAMERA_H_

//forward declarations
class Camera;
class Graphics;

#include "geometry/transform.h"
#include "graphics/render/renderbuffer.h"
#include "object/sceneobjectcomponent.h"

class Camera : public SceneObjectComponent
{
	friend class EngineObjectManager;

    unsigned int clearBufferMask;

    Transform projectionTransform;
    Graphics * graphics;

    protected:

    Camera(Graphics * graphics);
    ~Camera();

    public:

    const Transform& GetProjectionTransform() const ;
    void AddClearBuffer(RenderBufferType buffer);
    void RemoveClearBuffer(RenderBufferType buffer);
    unsigned int GetClearBufferMask() const;
};

#endif
