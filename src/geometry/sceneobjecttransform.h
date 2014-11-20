/*
 * class: SceneObjectTransform
 *
 * author: Mark Kellogg
 *
 * A SceneObjectTransform is an extension of Transform. A SceneObjectTransform is
 * different in that it is connected to a SceneObject, and whenever a transformation
 * is applied, the transforms of each ancestor of said SceneObject are factored in.
 */

#ifndef _SCENEOBJECT_TRANSFORM_H_
#define _SCENEOBJECT_TRANSFORM_H_

// forward declarations
class Transform;

#include "object/enginetypes.h"
#include "transform.h"
#include "matrix4x4.h"
#include "util/datastack.h"

class SceneObjectTransform : public Transform
{
	friend class SceneObject;

	SceneObjectRef sceneObject;
	Matrix4x4 localMatrix;

	void GetInheritedTransform(Transform& transform, bool invert);
	void UpdateTarget();
	void UpdateFullTransform();

    public:

	SceneObjectTransform();
	SceneObjectTransform(SceneObjectRef sceneObject);
	SceneObjectTransform(SceneObjectRef sceneObject, Matrix4x4& m);
	SceneObjectTransform(SceneObjectTransform& sceneObjectTransform);
    ~SceneObjectTransform();

    void AttachTo(SceneObjectRef sceneObject);
    void SetLocalTransform(const Transform& localTransform);
    void SetLocalMatrix(const Matrix4x4& localMatrix);

    void GetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale);
    void SetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale);

    void Translate(float x, float y, float z, bool local);
    void RotateAround(Point3& point, Vector3& axis, float angle);
    void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle);
    void Scale(Vector3& mag,  bool local);
    void Scale(float x, float y, float z,  bool local);
};

#endif
