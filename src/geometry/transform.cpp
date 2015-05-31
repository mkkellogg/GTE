#include <iostream>
#include <math.h>
#include <memory.h>
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
	* Instantiate a transform, which by default is an identity transform.
	*/
	Transform * Transform::CreateIdentityTransform()
	{
		return new Transform();
	}

	/*
	 * Default constructor
	 */
	Transform::Transform()
	{
		matrix.SetIdentity();
	}

	/*
	 * Copy constructor
	 */
	Transform::Transform(const Transform * transform) : Transform()
	{
		NONFATAL_ASSERT(transform != NULL, "Transform::copy constructor -> 'transform' is null.", true);
		transform->CopyMatrix(matrix);
	}

	/*
	 * Copy constructor
	 */
	Transform::Transform(const Transform& transform)
	{
		transform.CopyMatrix(matrix);
	}

	/*
	 * Clean up
	 */
	Transform::~Transform()
	{

	}

	/*
	 * Overloaded assignment operator
	 */
	Transform& Transform::operator= (const Transform& source)
	{
		if (this == &source)return *this;
		SetTo(source);
		return *this;
	}

	/*
	 * Copy this Transform object's matrix into [dest].
	 */
	void Transform::CopyMatrix(Matrix4x4& dest) const
	{
		dest.SetTo(matrix);
	}

	/*
	 * Copy [matrix] into the underlying Matrix4x4 that is encapsulated by this transform.
	 */
	void Transform::SetTo(const Matrix4x4& matrix)
	{
		this->matrix.SetTo(matrix);
	}

	/*
	 * Make this transform equal to [transform]
	 */
	void Transform::SetTo(const Transform& transform)
	{
		transform.CopyMatrix(matrix);
	}

	/*
	 * Set this transform's matrix data to [matrixData].
	 */
	void Transform::SetTo(const Real * matrixData)
	{
		NONFATAL_ASSERT(matrixData != NULL, "Transform::SetTo -> 'matrixData' is null.", true);
		matrix.SetTo(matrixData);
	}

	void Transform::SetIdentity()
	{
		matrix.SetIdentity();
	}

	/*
	 * Post-multiply this transform's matrix by [matrix]
	 */
	void Transform::TransformBy(const Matrix4x4& matrix)
	{
		this->matrix.Multiply(matrix);
	}

	/*
	 * Pre-multiply this transform's matrix by [matrix]
	 */
	void Transform::PreTransformBy(const Matrix4x4& matrix)
	{
		this->matrix.PreMultiply(matrix);
	}

	/*
	 * Post-multiply this transform's matrix by the matrix contained in [transform]
	 */
	void Transform::TransformBy(const Transform& transform)
	{
		matrix.Multiply(transform.matrix);
	}

	/*
	 * Pre-multiply this transform's matrix by the matrix contained in [transform]
	 */
	void Transform::PreTransformBy(const Transform& transform)
	{
		matrix.PreMultiply(transform.matrix);
	}

	/*
	 * Invert the underlying matrix
	 */
	void Transform::Invert()
	{
		matrix.Invert();
	}

	/*
	 * Apply translation transformation to this transform's matrix. The parameter [local]
	 * determines if the transformation is relative to world space or the transform's
	 * local space.
	 */
	void Transform::Translate(Real x, Real y, Real z, bool local)
	{
		if (!local)
		{
			matrix.PreTranslate(x, y, z);
		}
		else matrix.Translate(x, y, z);
	}

	/*
	 * Apply translation transformation to this transform's matrix. The parameter [local]
	 * determines if the transformation is relative to world space or the transform's
	 * local space.
	 */
	void Transform::Translate(Vector3& vector, bool local)
	{
		if (!local)
		{
			matrix.PreTranslate(vector);
		}
		else matrix.Translate(vector);
	}

	/*
	 * Rotate around a specific point and orientation vector.  The parameter [local] determines whether
	 * local or world space is used.
	 */
	void Transform::RotateAround(const Point3& point, const Vector3& axis, Real angle, bool local)
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
	* Post-multiplication operations are used to achieve the effect in local space, and pre-multiplication
	* is used to achieved the effect in local space. The parameter [local] determines which of those cases
	* to use.
	*/
	void Transform::RotateAround(Real px, Real py, Real pz, Real ax, Real ay, Real az, Real angle, bool local)
	{
		if (local)
		{
			matrix.Translate(px, py, pz);
			matrix.Rotate(ax, ay, az, angle);
			matrix.Translate(-px, -py, -pz);
		}
		else
		{
			matrix.PreTranslate(-px, -py, -pz);
			matrix.PreRotate(ax, ay, az, angle);
			matrix.PreTranslate(px, py, pz);
		}
	}

	/*
	 * Scale this transform by the x,y, and z components of [mag]
	 */
	void Transform::Scale(const Vector3& mag, bool local)
	{
		Scale(mag.x, mag.y, mag.z, local);
	}

	/*
	 * Scale this transform by [x], [y], [z]. If [local] is true then the operation is
	 * performed in local space, otherwise it is performed in world space.
	 */
	void Transform::Scale(Real x, Real y, Real z, bool local)
	{
		if (!local)
		{
			matrix.PreScale(x, y, z);
		}
		else matrix.Scale(x, y, z);
	}

	/*
	 * Rotate this transform around [vector]. If [local] is true, perform in
	 * local space, otherwise perform in world space.
	 */
	void Transform::Rotate(const Vector3& vector, Real a, bool local)
	{
		Rotate(vector.x, vector.y, vector.z, a, local);
	}

	/*
	 * Rotate this transform around the vector specified by [x], [y], [z].
	 * If [local] is true, perform in local space, otherwise perform in world space.
	 */
	void Transform::Rotate(Real x, Real y, Real z, Real a, bool local)
	{
		if (!local)
		{
			matrix.PreRotate(x, y, z, a);
		}
		else matrix.Rotate(x, y, z, a);
	}

	/*
	 * Utility function to create an orthographic projection matrix. An existing Matrix4x4 object is passed in via [matrix],
	 * and its data is set to contain the projection matrix.
	 *
	 * [top] 	- 	Distance from the eye of the top clip plane
	 * [bottom] 	-	Distance from the eye of the bottom clip plane
	 * [left]  -  Distance from the eye of the left clip plane
	 * [right]   -   Distance from the eye of the right clip plane
	 * [near]  -   Distance from the eye to the near clip plane
	 * [far]   -   Distance from the eye to the far clip plane
	 */
	void Transform::BuildOrthographicProjectionMatrix(Matrix4x4& matrix, Real top, Real bottom, Real left, Real right, Real near, Real far)
	{
		matrix.SetIdentity();

		Real data[16];
		memset(data, 0, 16 * sizeof(Real));

		Real r_width = 1.0f / (right - left);
		Real r_height = 1.0f / (top - bottom);
		Real r_depth = 1.0f / (far - near);
		Real x = 2.0f * (r_width);
		Real y = 2.0f * (r_height);
		Real z = -2.0f * (r_depth);
		Real tx = -(right + left) * r_width;
		Real ty = -(top + bottom) * r_height;
		Real tz = -(far + near) * r_depth;
		data[0] = x;
		data[5] = y;
		data[10] = z;
		data[12] = tx;
		data[13] = ty;
		data[14] = tz;
		data[15] = 1.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 0.0f;
		data[4] = 0.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;
		data[8] = 0.0f;
		data[9] = 0.0f;
		data[11] = 0.0f;

		matrix.SetTo(data);
	}

	/*
	 * Utility function to create a perspective projection matrix. An existing Matrix4x4 object is passed in via [matrix],
	 * and its data is set to contain the projection matrix.
	 *
	 * [fov] 	- 	Angle (in degrees) of the field of view.
	 * [ratio] 	-	Ratio of the viewport's width to height
	 * [nearP]  -   Distance from the eye to the near clip plane
	 * [farP]   -   Distance from the eye to the far clip plane
	 */
	void Transform::BuildPerspectiveProjectionMatrix(Matrix4x4& matrix, Real fov, Real ratio, Real nearP, Real farP)
	{
		// convert fov to radians
		Real f = 1.0f / tan(fov * Constants::TwoPIOver360 *.5);

		matrix.SetIdentity();

		Real data[16];
		memset(data, 0, 16 * sizeof(Real));

		data[0] = f / ratio;
		data[1 * 4 + 1] = f;
		data[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
		data[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
		data[2 * 4 + 3] = -1.0f;
		data[3 * 4 + 3] = 0.0f;
		matrix.SetTo(data);
	}

	/*
	 * Utility function to create a perspective projection matrix with an infinite far plane.
	 * An existing Matrix4x4 object is passed in via [matrix], and its data is set to contain the projection matrix.
	 *
	 * [fov] 	- 	Angle (in degrees) of the field of view.
	 * [ratio] 	-	Ratio of the viewport's width to height
	 * [nearP]  -   Distance from the eye to the near clip plane
	 */
	void Transform::BuildPerspectiveProjectionMatrixInfiniteFar(Matrix4x4& matrix, Real fov, Real ratio, Real nearP)
	{
		// convert fov to radians
		Real f = 1.0f / tan(fov * Constants::TwoPIOver360 *.5);

		matrix.SetIdentity();

		Real data[16];
		memset(data, 0, 16 * sizeof(Real));

		data[0] = f / ratio;
		data[1 * 4 + 1] = f;
		data[2 * 4 + 2] = -1.0;
		data[3 * 4 + 2] = -2.0 * nearP;
		data[2 * 4 + 3] = -1.0f;
		data[3 * 4 + 3] = 0.0f;
		matrix.SetTo(data);
	}

	/*
	 * Utility function to create a matrix that transforms the eye to be at a specific
	 * location, looking in a specific direction. The direction vector does not have
	 * to be normalized.
	 *
	 * [matrix]  -	The existing matrix to which the transformation data will be copied.
	 *
	 * [posX]    -  The X coordinate of the look at location
	 * [posY]    -  The Y coordinate of the look at location
	 * [posZ]    -  The Z coordinate of the look at location
	 *
	 * [lookAtX]    -  The X value of the look at direction
	 * [lookAtY]    -  The Y value of the look at direction
	 * [lookAtZ]    -  The Z value of the look at direction
	 */
	void Transform::BuildLookAtMatrix(Matrix4x4& matrix, Real posX, Real posY, Real posZ,
		Real lookAtX, Real lookAtY, Real lookAtZ)
	{
		Vector3 vDir(lookAtX - posX, lookAtY - posY, lookAtZ - posZ);
		Vector3 vUp(0, 1, 0);
		Vector3 vRight;

		vDir.Normalize();

		Vector3::Cross(vDir, vUp, vRight);
		vRight.Normalize();

		Vector3::Cross(vRight, vDir, vUp);
		vUp.Normalize();

		Real view[16];

		view[0] = vRight.x;
		view[4] = vRight.y;
		view[8] = vRight.z;
		view[12] = 0.0f;

		view[1] = vUp.x;
		view[5] = vUp.y;
		view[9] = vUp.z;
		view[13] = 0.0f;

		view[2] = -vDir.x;
		view[6] = -vDir.y;
		view[10] = -vDir.z;
		view[14] = 0.0f;

		view[3] = 0.0f;
		view[7] = 0.0f;
		view[11] = 0.0f;
		view[15] = 1.0f;

		Matrix4x4 viewMatrix;
		viewMatrix.SetTo(view);

		Matrix4x4 aux;

		aux.SetIdentity();
		aux.Translate(-posX, -posY, -posZ);

		Matrix4x4::Multiply(viewMatrix, aux, matrix);
	}

	/*
	 * Shortcut to transform [vector] by [matrix]. This performs a
	 * local transformation via post-multiplication.
	 */
	void Transform::TransformVector(Vector3& vector) const
	{
		matrix.Transform(vector);
	}

	/*
	 * Shortcut to transform [point] by [matrix]. This performs a
	 * local transformation via post-multiplication.
	 */
	void Transform::TransformPoint(Point3& point) const
	{
		matrix.Transform(point);
	}

	/*
	 * Shortcut to transform [vector] by [matrix]. This performs a
	 * local transformation via post-multiplication.
	 */
	void Transform::TransformVector4f(Real * vector) const
	{
		NONFATAL_ASSERT(vector != NULL, "Transform::TransformVector4f -> 'vector' is null.", true);
		matrix.Transform(vector);
	}
}
