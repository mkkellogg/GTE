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

	SceneObject * sceneObject;

	void GetInheritedTransform(Transform& transform, bool invert);
	void SetSceneObject(SceneObject* sceneObject);

    public:

	SceneObjectTransform();
	SceneObjectTransform(SceneObject* sceneObject);
	SceneObjectTransform(SceneObject* sceneObject, Matrix4x4& m);
    ~SceneObjectTransform();

    static void GetWorldTransform(Transform& transform, SceneObject * sceneObject, bool includeSelf, bool invert);
    static void GetWorldTransform(Transform& transform, SceneObjectRef sceneObject, bool includeSelf,  bool invert);

    void GetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale);
    void SetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale);

    void Translate(float x, float y, float z, bool local);
    void Translate(Vector3& vector, bool local);
    void RotateAround(const Point3& point, const Vector3& axis, float angle, bool local);
    void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle, bool local);
    void Scale(const Vector3& mag,  bool local);
    void Scale(float x, float y, float z,  bool local);
};

#endif
