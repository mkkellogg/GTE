#ifndef _SCENEOBJECT_TRANSFORM_H_
#define _SCENEOBJECT_TRANSFORM_H_

// forward declarations
class Transform;
class SceneObject;
class SceneObjectTransform;

#include "transform.h"
#include "matrix4x4.h"
#include "util/datastack.h"

class SceneObjectTransform : public Transform
{
	SceneObject * sceneObject;

	void GetInheritedTransform(Transform * transform, bool invert);

    public:

	SceneObjectTransform(SceneObject * sceneObject);
	SceneObjectTransform(SceneObject * sceneObject, Matrix4x4 * m);
	SceneObjectTransform(SceneObject * sceneObject, SceneObjectTransform * sceneObjectTransform);
    ~SceneObjectTransform();

    void Translate(float x, float y, float z, bool local);
    void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle);

};

#endif
