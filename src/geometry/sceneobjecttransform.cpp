#include <iostream>
#include <math.h>
#include <memory.h>
#include "sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "transform.h"
#include "matrix4x4.h"
#include "ui/debug.h"
#include "global/constants.h"
#include "global/global.h"
#include "vector/vector3.h"
#include "point/point3.h"
#include "util/datastack.h"

/*
 * Default constructor
 */
SceneObjectTransform::SceneObjectTransform() : Transform()
{
	sceneObject = NULL;
}

/*
 * Base constructor
 */
SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject) : Transform()
{
	this->sceneObject = sceneObject;
}

/*
 * Constructor to build SceneObjectTransform from Matrix4x4
 */
SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject, Matrix4x4& m) : Transform(m)
{
	this->sceneObject = sceneObject;
}

/*
 * Clean up
 */
SceneObjectTransform::~SceneObjectTransform()
{

}

/*
 * Decompose this transform's matrix into [translation], [rotation], and [scaling].
 */
void SceneObjectTransform::GetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale)
{
	matrix.Decompose(translation, rotation, scale);
}

/*
 * Build matrix from base components: [translation], [rotation], and [scaling].
 */
void SceneObjectTransform::SetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale)
{
	matrix.BuildFromComponents(translation, rotation, scale);
}

/*
 * Walk up the scene object tree, beginning with the parent of this transform's
 * scene object, and concatenate each ancestor's transform to form the transform
 * that is inherited by connected scene object.
 */
void SceneObjectTransform::GetInheritedTransform(Transform& transform, bool invert)
{
	GetWorldTransform(transform, sceneObject, false, invert);
}

/*
 * Walk up the scene object tree, beginning with the parent of [sceneObject], and
 * concatenate each ancestor's transform to form the transform that is inherited by
 * [sceneObject].
 */
void SceneObjectTransform::GetWorldTransform(Transform& transform, SceneObjectRef sceneObject, bool includeSelf, bool invert)
{
	ASSERT_RTRN(sceneObject.IsValid(), "SceneObjectTransform::GetWorldTransform() -> sceneObject is not valid.");
	GetWorldTransform(transform, sceneObject.GetPtr(), includeSelf, invert);
}

/*
 * Walk up the scene object tree, beginning with the parent of [sceneObject], and
 * concatenate each ancestor's transform to form the transform that is inherited by
 * [sceneObject].
 */
void SceneObjectTransform::GetWorldTransform(Transform& transform, SceneObject * sceneObject, bool includeSelf, bool invert )
{
	ASSERT_RTRN(sceneObject != NULL, "SceneObjectTransform::GetWorldTransform() -> sceneObject is NULL.");

	Transform full;
	if(includeSelf)full.SetTo(sceneObject->GetTransform());
	SceneObjectRef parent = sceneObject->GetParent();
	while(parent.IsValid())
	{
		// Since we are processing the ancestors in reverse order (going up the tree)
		// we pre-multiply, to have the end effect of post-multiplication in
		// the normal order
		full.PreTransformBy(parent->GetTransform());
		parent = parent->GetParent();
	}
	// optionally invert
	if(invert == true)full.Invert();
	transform.SetTo(full);
}

/*
 * Set the target SceneObject for this transform;
 */
void SceneObjectTransform::SetSceneObject(SceneObject* sceneObject)
{
	this->sceneObject = sceneObject;
}

/*
 * Apply translation transformation to this transform's matrix. The parameter [local]
 * determines if the transformation is relative to world coordinates or the transform's
 * local space, which includes the aggregated transform of the scene object's ancestors.
 */
void SceneObjectTransform::Translate(float x, float y, float z, bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		GetInheritedTransform(full, false);
		full.TransformBy(this);
		full.Invert();
		full.TransformVector4f(trans);

		matrix.Translate(trans[0], trans[1], trans[2]);
	}
	else matrix.Translate(x,y,z);
}

/*
 * Apply translation transformation to this transform's matrix. The parameter [local]
 * determines if the transformation is relative to world coordinates or the transform's
 * local space, which includes the aggregated transform of the scene object's ancestors.
 */
void SceneObjectTransform::Translate(Vector3& vector, bool local)
{
	Translate(vector.x, vector.y, vector.z, local);
}

/*
 * Rotate around a specific point and orientation vector. The parameter [local] determines whether
 * local or world space is used.
 */
void SceneObjectTransform::RotateAround(const Point3& point, const Vector3& axis, float angle, bool local)
{
	RotateAround(point.x, point.y, point.z, axis.x, axis.y, axis.z, angle, local);
}

/*
* Rotate around a specific point and orientation vector.
*
* The point is specified by [px], [py], and [pz].
*
* The orientation vector is specified by [ax], [ay], and [az].
*
* The parameter [local] determines whether local or world space is used.
* If world space is used, it takes into account the  aggregated transform
* of the scene object's ancestors.
*/
void SceneObjectTransform::RotateAround(float px, float py, float pz, float ax, float ay, float az, float angle, bool local)
{
	if(!local)
	{
		float pointTrans[] = {px,py,pz,1};
		float rotVector[] = {ax,ay,az,0};

		Transform mod;
		GetInheritedTransform(mod, false);
		mod.TransformBy(this);

		mod.Invert();
		mod.TransformVector4f(pointTrans);
		mod.TransformVector4f(rotVector);

		float diffX = pointTrans[0];
		float diffY = pointTrans[1];
		float diffZ = pointTrans[2];

		/*matrix.Translate(diffX,diffY,diffZ);
		matrix.Rotate(rotVector[0],rotVector[1],rotVector[2],angle);
		matrix.Translate(-diffX,-diffY,-diffZ);*/

		matrix.PreTranslate(-px,-py,-pz);
		matrix.PreRotate(ax,ay,az,angle);
		matrix.PreTranslate(px,py,pz);
	}
	else
	{
		matrix.PreTranslate(-px,-py,-pz);
		matrix.PreRotate(ax,ay,az,angle);
		matrix.PreTranslate(px,py,pz);
	}
}

/*
 * Scale this transform by the x,y, and z components of [mag]
 */
void SceneObjectTransform::Scale(const Vector3& mag,  bool local)
{
	Scale(mag.x, mag.y, mag.z, local);
}

/*
 * Scale this transform by [x], [y], [z]. If [local] is true then the operation is
 * performed in local space, otherwise it is performed in global space.
 */
void SceneObjectTransform::Scale(float x, float y, float z,  bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		GetInheritedTransform(full, false);
		full.TransformBy(this);
		full.Invert();
		full.TransformVector4f(trans);

		matrix.Scale(trans[0], trans[1], trans[2]);
	}
	else matrix.Scale(x,y,z);
}

