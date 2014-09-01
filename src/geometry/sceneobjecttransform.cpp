#include <iostream>
#include <math.h>
#include <memory.h>
#include "sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "transform.h"
#include "matrix4x4.h"
#include "ui/debug.h"
#include "global/constants.h"
#include "global/global.h"
#include "vector/vector3.h"
#include "point/point3.h"
#include "util/datastack.h"

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
SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject, Matrix4x4 * m) : Transform(m)
{
	this->sceneObject = sceneObject;
}

/*
 * Copy constructor
 */
SceneObjectTransform::SceneObjectTransform(SceneObject * sceneObject, SceneObjectTransform * sceneObjectTransform) :  Transform(sceneObjectTransform)
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
 * This method is used to make transformations relative to the transformations of
 * all ancestors of the connected scene object.
 *
 * Walk up the scene object tree, beginning with the parent of this transform's
 * scene object, and concatenate each ancestor's transform to form the transform
 * that is inherited by connected scene object.
 */
void SceneObjectTransform::GetInheritedTransform(Transform * transform, bool invert)
{
	NULL_CHECK_RTRN(transform, "SceneObjectTransform::GetInheritedTransform -> NULL transform passed.");

	Transform full;
	SceneObject * parent = sceneObject->GetParent();
	while(parent != NULL)
	{
		// Since we are processing the ancestors in reverse order (going up the tree)
		// we pre-multiply, to have the end effect of post-multiplication in
		// the normal order
		full.PreTransformBy(parent->GetTransform());
		parent = parent->GetParent();
	}
	// optionally invert
	if(invert == true)full.Invert();
	transform->SetTo(&full);
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
		//matrix.PreTranslate(x,y,z);

		float trans[] = {x,y,z,0};
		Transform full;
		GetInheritedTransform(&full, false);
		full.TransformBy(this);
		full.Invert();
		full.GetMatrix()->Transform(trans);

		matrix.Translate(trans[0], trans[1], trans[2]);
	}
	else matrix.Translate(x,y,z);
}

/*
 * Rotate around a specific world point and orientation vector.
 */
void SceneObjectTransform::RotateAround(Point3 * point, Vector3 * axis, float angle)
{
	RotateAround(point->x, point->y, point->z, axis->x, axis->y, axis->z, angle);
}

/*
* Rotate around a specific world point and orientation vector.
*
* The world point is specified by [px], [py], and [pz].
*
* The orientation vector is specified by [ax], [ay], and [az].
*
* This is a world space operation, that takes into account the  aggregated transform
* of the scene object's ancestors.
*/
void SceneObjectTransform::RotateAround(float px, float py, float pz, float ax, float ay, float az, float angle)
{
	/*matrix.PreTranslate(-px,-py,-pz);
	matrix.PreRotate(ax,ay,az,angle);
	matrix.PreTranslate(px,py,pz);*/

	float pointTrans[] = {px,py,pz,1};
	float rotVector[] = {ax,ay,az,0};

	Transform mod;
	GetInheritedTransform(&mod, false);
	mod.TransformBy(this);

	mod.Invert();
	mod.GetMatrix()->Transform(pointTrans);
	mod.GetMatrix()->Transform(rotVector);

	float diffX = pointTrans[0];
	float diffY = pointTrans[1];
	float diffZ = pointTrans[2];

	matrix.Translate(diffX,diffY,diffZ);
	matrix.Rotate(rotVector[0],rotVector[1],rotVector[2],angle);
	matrix.Translate(-diffX,-diffY,-diffZ);
}

/*
 * Scale this transform by the x,y, and z components of [mag]
 */
void SceneObjectTransform::Scale(Vector3 * mag,  bool local)
{
	NULL_CHECK_RTRN(mag, "Transform::Scale -> mag is null.");

	Scale(mag->x, mag->y, mag->z, local);
}

/*
 * Scale this transform by [x], [y], [z]. If [local] is true then the operation is
 * performed in local space, otherwise it is performed in local space.
 */
void SceneObjectTransform::Scale(float x, float y, float z,  bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		GetInheritedTransform(&full, false);
		full.TransformBy(this);
		full.Invert();
		full.GetMatrix()->Transform(trans);

		matrix.Scale(trans[0], trans[1], trans[2]);
	}
	else matrix.Scale(x,y,z);
}

