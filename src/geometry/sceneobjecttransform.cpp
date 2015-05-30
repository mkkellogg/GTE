#include <iostream>
#include <math.h>
#include <memory.h>
#include "sceneobjecttransform.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "transform.h"
#include "matrix4x4.h"
#include "debug/gtedebug.h"
#include "global/constants.h"
#include "global/global.h"
#include "vector/vector3.h"
#include "point/point3.h"
#include "util/datastack.h"

namespace GTE
{
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
	 * Clean up
	 */
	SceneObjectTransform::~SceneObjectTransform()
	{

	}

	/*
	 * Decompose this transform's matrix into [translation], [rotation], and [scaling].
	 */
	void SceneObjectTransform::GetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale) const
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
	void SceneObjectTransform::GetInheritedTransform(Transform& transform, bool invert) const
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
		NONFATAL_ASSERT(sceneObject.IsValid(), "SceneObjectTransform::GetWorldTransform() -> 'sceneObject' is not valid.", true);
		GetWorldTransform(transform, sceneObject.GetPtr(), includeSelf, invert);
	}

	/*
	 * Walk up the scene object tree, beginning with the parent of [sceneObject], and
	 * concatenate each ancestor's transform to form the transform that is inherited by
	 * [sceneObject].
	 */
	void SceneObjectTransform::GetWorldTransform(Transform& transform, SceneObject * sceneObject, bool includeSelf, bool invert)
	{
		NONFATAL_ASSERT(sceneObject != NULL, "SceneObjectTransform::GetWorldTransform() -> 'sceneObject' is null.", true);

		Transform full;
		if (includeSelf)full.SetTo(sceneObject->GetTransform());
		SceneObjectRef parent = sceneObject->GetParent();
		while (parent.IsValid())
		{
			// Since we are processing the ancestors in reverse order (going up the tree)
			// we pre-multiply, to have the end effect of post-multiplication in
			// the normal order
			full.PreTransformBy(parent->GetTransform());
			parent = parent->GetParent();
		}
		// optionally invert
		if (invert == true)full.Invert();
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
	 * This method plays a critical part of performing transformations on scene objects in world space. In order to perform
	 * these kinds of transformations, it is necessary to take into account each local transformation of each ancestor of
	 * the scene object. If we wanted to apply a world transformation to a single matrix, we would simply pre-multiply that
	 * matrix with the desired transformation. With scene objects that are part of a scene hierarchy, we can't do that since
	 * the pre-multiplication would have to occur at the top of the hierarchy, and therefore quite likely to a different scene
	 * object than the one in question (we only want to modify the transform of the target scene object).
	 *
	 * We solve this problem by doing some arithmetic to find the equivalent transformation in the scene object's local space
	 * that accomplishes the same effect as the world space transformation that would occur on the scene object at the top of
	 * the hierarchy:
	 *
	 *   S = The target scene object.
	 *   A = Aggregate/concatenation of all ancestors of S.
	 *   L = The local transformation of S.
	 *   nWorld = The world space transformation.
	 *   nLocal = The transformation in the local space of S.
	 *
	 *   F = The concatenation of A & L -> A * L
	 *   FI = The inverse of F.
	 *
	 *   We can easily derive a desired world-space transformation that is suited for pre-multiplication. To apply that transformation,
	 *   we could simply do: nWorld * F. The problem there is that we'd have to apply that transformation to the top of the hierarchy,
	 *   which we cannot do as it would likely affect other scene objects. We find the equivalent transformation in the local space of S (nLocal) by:
	 *
	 *   	  nWorld * F = F * nLocal
	 *   FI * nWorld * F = FI * F * nLocal
	 *   			     = nLocal
	 *
	 *  Therefore the equivalent transformation in the local space of S is: FI * nWorld * F. This method takes in nWorld [worldTransformation]
	 *  and produces (FI * nWorld * F) in [localTransformation].
	 */
	void SceneObjectTransform::GetLocalTransformationFromWorldTransformation(const Transform& worldTransformation, Transform& localTransformation)
	{
		Transform fullInverse;
		GetInheritedTransform(localTransformation, false);
		localTransformation.TransformBy(this);
		fullInverse.SetTo(localTransformation);
		fullInverse.Invert();
		localTransformation.PreTransformBy(worldTransformation);
		localTransformation.PreTransformBy(fullInverse);
	}

	/*
	 * Apply translation transformation to this transform's matrix. The parameter [local]
	 * determines if the transformation is relative to world coordinates or the transform's
	 * local space, which includes the aggregated transform of the scene object's ancestors.
	 */
	void SceneObjectTransform::Translate(float x, float y, float z, bool local)
	{
		if (!local)
		{
			Matrix4x4 transMat;
			transMat.PreTranslate(x, y, z);
			Transform worldTrans;
			worldTrans.SetTo(transMat);

			Transform localTrans;
			GetLocalTransformationFromWorldTransformation(worldTrans, localTrans);

			this->TransformBy(localTrans);
		}
		else
		{
			Transform::Translate(x, y, z, true);
		}
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
	 * local or world space is used. If world space is used, it takes into account the  aggregated transform
	 * of the scene object's ancestors.
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
		if (!local)
		{
			Matrix4x4 worldTransMat;
			worldTransMat.PreTranslate(-px, -py, -pz);
			worldTransMat.PreRotate(ax, ay, az, angle);
			worldTransMat.PreTranslate(px, py, pz);
			Transform worldTrans;
			worldTrans.SetTo(worldTransMat);

			Transform localTrans;
			GetLocalTransformationFromWorldTransformation(worldTrans, localTrans);

			this->TransformBy(localTrans);
		}
		else
		{
			Transform::RotateAround(px, py, pz, ax, ay, az, angle, true);
		}
	}

	/*
	 * Scale this transform by the x,y, and z components of [mag]. If [local] is true then
	 * the operation is performed in local space, otherwise it is performed in world space.
	 * If world space is used, it takes into account the  aggregated transform of the scene
	 * object's ancestors.
	 */
	void SceneObjectTransform::Scale(const Vector3& mag, bool local)
	{
		Scale(mag.x, mag.y, mag.z, local);
	}

	/*
	 * Scale this transform by [x], [y], and [z]. If [local] is true then
	 * the operation is performed in local space, otherwise it is performed in world space.
	 * If world space is used, it takes into account the  aggregated transform of the scene
	 * object's ancestors.
	 */
	void SceneObjectTransform::Scale(float x, float y, float z, bool local)
	{
		if (!local)
		{
			Matrix4x4 scaleMat;
			scaleMat.PreScale(x, y, z);
			Transform worldTrans;
			worldTrans.SetTo(scaleMat);

			Transform localTrans;
			GetLocalTransformationFromWorldTransformation(worldTrans, localTrans);

			this->TransformBy(localTrans);
		}
		else
		{
			Transform::Scale(x, y, z, true);
		}
	}

	/*
	 * Rotate this transform around [vector]. If [local] is true, perform in
	 * local space, otherwise perform in world space. If world space is used, it
	 * takes into account the  aggregated transform of the scene object's ancestors.
	 */
	void SceneObjectTransform::Rotate(const Vector3& vector, float a, bool local)
	{
		Rotate(vector.x, vector.y, vector.z, a, local);
	}

	/*
	 * Rotate this transform around the vector specified by [x], [y], [z].
	 * If [local] is true, perform in local space, otherwise perform in world space. If
	 * world space is used, it takes into account the  aggregated transform of the scene
	 * object's ancestors.
	 */
	void SceneObjectTransform::Rotate(float x, float y, float z, float a, bool local)
	{
		if (!local)
		{
			Matrix4x4 rotMat;
			rotMat.PreRotate(x, y, z, a);
			Transform worldTrans;
			worldTrans.SetTo(rotMat);

			Transform localTrans;
			GetLocalTransformationFromWorldTransformation(worldTrans, localTrans);

			this->TransformBy(localTrans);
		}
		else
		{
			Transform::Rotate(x, y, z, a, true);
		}
	}

	/*
	 * Shortcut to transform [vector]  by [matrix]. This transformation occurs in world space.
	 */
	void SceneObjectTransform::TransformVector(Vector3& vector) const
	{
		Transform full;
		GetInheritedTransform(full, false);
		full.TransformBy(this);
		full.TransformVector(vector);
	}

	/*
	 * Shortcut to transform [point]  by [matrix]. This transformation occurs in world space.
	 */
	void SceneObjectTransform::TransformPoint(Point3& point) const
	{
		Transform full;
		GetInheritedTransform(full, false);
		full.TransformBy(this);
		full.TransformPoint(point);
	}

	/*
	 * Shortcut to transform [vector] by [matrix]. This transformation occurs in world space.
	 */
	void SceneObjectTransform::TransformVector4f(float * vector) const
	{
		NONFATAL_ASSERT(vector != NULL, "SceneObjectTransform::TransformVector4f -> 'vector' is null.", true);

		Transform full;
		GetInheritedTransform(full, false);
		full.TransformBy(this);
		full.TransformVector4f(vector);
	}
}
