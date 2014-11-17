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

}

/*
 * Base constructor
 */
SceneObjectTransform::SceneObjectTransform(SceneObjectRef sceneObject) : Transform()
{
	this->sceneObject = sceneObject;
}

/*
 * Constructor to build SceneObjectTransform from Matrix4x4
 */
SceneObjectTransform::SceneObjectTransform(SceneObjectRef sceneObject, Matrix4x4 * m) : Transform(m)
{
	this->sceneObject = sceneObject;
}

/*
 * Copy constructor
 */
SceneObjectTransform::SceneObjectTransform(SceneObjectTransform * sceneObjectTransform) :  Transform(sceneObjectTransform)
{
	this->sceneObject = sceneObjectTransform->sceneObject;
}

/*
 * Clean up
 */
SceneObjectTransform::~SceneObjectTransform()
{

}

/*
 * Set the target SceneObject.
 */
void SceneObjectTransform::AttachTo(SceneObjectRef sceneObject)
{
	this->sceneObject = sceneObject;
}


/*
 * Set the local matrix [localMatrix] to match [localTransform].
 */
void SceneObjectTransform::SetLocalTransform(const Transform* localTransform)
{
	localTransform->CopyMatrix(&this->localMatrix);

	UpdateFullTransform();
	UpdateTarget();
}

/*
 * Set the local matrix.
 */
void SceneObjectTransform::SetLocalMatrix(const Matrix4x4 *  localMatrix)
{
	this->localMatrix.SetTo(localMatrix);

	UpdateFullTransform();
	UpdateTarget();
}

/*
 * Decompose this transform's matrix into [translation], [rotation], and [scaling].
 */
void SceneObjectTransform::GetLocalComponents(Vector3 * translation, Quaternion * rotation, Vector3 * scale)
{
	localMatrix.Decompose(translation, rotation, scale);
}

/*
 * Build matrix from base components: [translation], [rotation], and [scaling].
 */
void SceneObjectTransform::SetLocalComponents(Vector3 * translation, Quaternion * rotation, Vector3 * scale)
{
	localMatrix.BuildFromComponents(translation, rotation, scale);

	UpdateFullTransform();
	UpdateTarget();
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
	ASSERT_RTRN(transform != NULL, "SceneObjectTransform::GetInheritedTransform -> NULL transform passed.");

	Transform full;
	SceneObjectRef parent = sceneObject->GetParent();
	while(parent.IsValid())
	{
		// Since we are processing the ancestors in reverse order (going up the tree)
		// we pre-multiply, to have the end effect of post-multiplication in
		// the normal order
		full.PreTransformBy(parent->GetLocalTransform());
		parent = parent->GetParent();
	}
	// optionally invert
	if(invert == true)full.Invert();
	transform->SetTo(&full);
}

/*
 * Update the local transform of the target SceneObject to reflect any relevant changes.
 */
void SceneObjectTransform::UpdateTarget()
{
	sceneObject->GetLocalTransform().SetTo(&localMatrix);
}

/*
 * Update the the value of this SceneObjectTransform object's full matrix [matrix]. This
 * should be called after any operation that affects [localMatrix].
 */
void SceneObjectTransform::UpdateFullTransform()
{
	Transform full;
	GetInheritedTransform(&full, false);
	full.TransformBy(&this->localMatrix);
	SetTo(&full);
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
		full.TransformVector4f(trans);

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
	mod.TransformVector4f(pointTrans);
	mod.TransformVector4f(rotVector);

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
	ASSERT_RTRN(mag != NULL, "Transform::Scale -> mag is null.");

	Scale(mag->x, mag->y, mag->z, local);
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
		GetInheritedTransform(&full, false);
		full.TransformBy(this);
		full.Invert();
		full.TransformVector4f(trans);

		matrix.Scale(trans[0], trans[1], trans[2]);
	}
	else matrix.Scale(x,y,z);
}

