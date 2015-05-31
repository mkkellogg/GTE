/*
 * class: SceneObjectTransform
 *
 * author: Mark Kellogg
 *
 * A SceneObjectTransform is an extension of Transform. A SceneObjectTransform is
 * different in that it is connected to a SceneObject, and whenever a world-space
 * transformation occurs, the transforms of each ancestor of said SceneObject are factored in.
 */

#ifndef _GTE_SCENEOBJECT_TRANSFORM_H_
#define _GTE_SCENEOBJECT_TRANSFORM_H_

#include "object/enginetypes.h"
#include "global/global.h"
#include "transform.h"
#include "matrix4x4.h"

namespace GTE
{
	// forward declarations
	class Transform;

	class SceneObjectTransform : public Transform
	{
		friend class SceneObject;

		SceneObject * sceneObject;

		void GetInheritedTransform(Transform& transform, Bool invert) const;
		void SetSceneObject(SceneObject* sceneObject);
		void GetLocalTransformationFromWorldTransformation(const Transform& worldTransformation, Transform& localTransformation);

	public:

		SceneObjectTransform();
		SceneObjectTransform(SceneObject* sceneObject);
		~SceneObjectTransform();

		static void GetWorldTransform(Transform& transform, SceneObject * sceneObject, Bool includeSelf, Bool invert);
		static void GetWorldTransform(Transform& transform, SceneObjectRef sceneObject, Bool includeSelf, Bool invert);

		void GetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale) const;
		void SetLocalComponents(Vector3& translation, Quaternion& rotation, Vector3& scale);

		void Translate(Real x, Real y, Real z, Bool local);
		void Translate(Vector3& vector, Bool local);
		void RotateAround(const Point3& point, const Vector3& axis, Real angle, Bool local);
		void RotateAround(Real px, Real py, Real pz, Real ax, Real ay, Real az, Real angle, Bool local);
		void Scale(const Vector3& mag, Bool local);
		void Scale(Real x, Real y, Real z, Bool local);
		void Rotate(const Vector3& vector, Real a, Bool local);
		void Rotate(Real x, Real y, Real z, Real a, Bool local);

		void TransformVector(Vector3& vector) const;
		void TransformPoint(Point3& point3) const;
		void TransformVector4f(Real * vector) const;
	};
}

#endif
