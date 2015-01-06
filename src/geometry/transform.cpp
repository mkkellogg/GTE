#include <iostream>
#include <math.h>
#include <memory.h>
#include "transform.h"
#include "matrix4x4.h"
#include "ui/debug.h"
#include "global/constants.h"
#include "global/global.h"
#include "vector/vector3.h"
#include "point/point3.h"
#include "util/datastack.h"

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
 * Constructor to build Transform from Matrix4x4
 */
Transform::Transform(const Matrix4x4& m) : Transform()
{
	matrix.SetTo(m);
}

/*
 * Copy constructor
 */
Transform::Transform(Transform * transform) : Transform()
{
	ASSERT_RTRN(transform != NULL, "Transform::copy constructor -> NULL transform passed.");
	transform->CopyMatrix(matrix);
}

/*
 * Clean up
 */
Transform::~Transform()
{

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
void Transform::SetTo(const float * matrixData)
{
	ASSERT_RTRN(matrixData != NULL, "Transform::SetTo -> NULL matrix data passed.");
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
void Transform::Translate(float x, float y, float z, bool local)
{
	if(!local)
	{
		/*float trans[] = {x,y,z,0};
		Transform full;
		full.SetTo(this);
		full.Invert();
		full.GetMatrix()->Transform(trans);
		matrix.Translate(trans[0], trans[1], trans[2]);*/

		matrix.PreTranslate(x,y,z);
	}
	else matrix.Translate(x,y,z);
}

/*
 * Apply translation transformation to this transform's matrix. The parameter [local]
 * determines if the transformation is relative to world space or the transform's
 * local space.
 */
void Transform::Translate(Vector3& vector, bool local)
{
	if(!local)
	{
		/*float trans[] = {x,y,z,0};
		Transform full;
		full.SetTo(this);
		full.Invert();
		full.GetMatrix()->Transform(trans);
		matrix.Translate(trans[0], trans[1], trans[2]);*/

		matrix.PreTranslate(vector);
	}
	else matrix.Translate(vector);
}

/*
 * Rotate around a specific local point and orientation vector.
 */
void Transform::RotateAround(const Point3& point, const Vector3& axis, float angle)
{
	RotateAround(point.x, point.y, point.z, axis.x, axis.y, axis.z, angle);
}

 /*
 * Rotate around a specific local point and orientation vector.
 *
 * The local point is specified by [px], [py], and [pz].
 *
 * The orientation vector is specified by [ax], [ay], and [az].
 *
 * Post-multiplication operations are used to achieve the effect in local space.
 */
void Transform::RotateAround(float px, float py, float pz, float ax, float ay, float az, float angle)
{
	/*float pointTrans[] = {px,py,pz,1};
	float rotVector[] = {ax,ay,az,0};

	Transform mod;
	mod.SetTo(this);

	mod.Invert();
	mod.GetMatrix()->Transform(pointTrans);
	mod.GetMatrix()->Transform(rotVector);

	float diffX = pointTrans[0];
	float diffY = pointTrans[1];
	float diffZ = pointTrans[2];

	matrix.Translate(diffX,diffY,diffZ);
	matrix.Rotate(rotVector[0],rotVector[1],rotVector[2],angle);
	matrix.Translate(-diffX,-diffY,-diffZ);*/

	/*matrix.PreTranslate(-px,-py,-pz);
	matrix.PreRotate(ax,ay,az,angle);
	matrix.PreTranslate(px,py,pz);*/

	matrix.Translate(px,py,pz);
	matrix.Rotate(ax,ay,az,angle);
	matrix.Translate(-px,-py,-pz);
}

/*
 * Scale this transform by the x,y, and z components of [mag]
 */
void Transform::Scale(const Vector3& mag,  bool local)
{
	Scale(mag.x, mag.y, mag.z, local);
}

/*
 * Scale this transform by [x], [y], [z]. If [local] is true then the operation is
 * performed in local space, otherwise it is performed in world space.
 */
void Transform::Scale(float x, float y, float z,  bool local)
{
	if(!local)
	{
		matrix.PreScale(x,y,z);
	}
	else matrix.Scale(x,y,z);
}

void Transform::Rotate(const Vector3& vector, float a)
{
	matrix.Rotate(vector, a);
}

void Transform::Rotate(float x, float y, float z, float a)
{
	matrix.Rotate(x,y,z,a);
}

/*
 * Utility function to create a projection matrix. An existing Matrix4x4 object is passed in via [matrix],
 * and its data is set to contain the projection matrix.
 *
 * [fov] 	- 	Angle (in degrees) of the field of view.
 * [ratio] 	-	Ration of the viewport's width to height
 * [nearP]  -   Distance from the eye to the near clip plane
 * [farP]   -   Distance from the eye to the far clip plane
 */
void Transform::BuildProjectionMatrix(Matrix4x4& matrix, float fov, float ratio, float nearP, float farP)
{
	// convert fov to radians
    float f = 1.0f / tan (fov * Constants::TwoPIOver360 *.5);

    matrix.SetIdentity();

    float data[16];
    memset(data,0,16 * sizeof(float));

    data[0] = f / ratio;
    data[1 * 4 + 1] = f;
    data[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    data[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    data[2 * 4 + 3] = -1.0f;
    data[3 * 4 + 3] = 0.0f;
    matrix.SetTo(data);
}

void Transform::BuildProjectionMatrixInfiniteFar(Matrix4x4& matrix, float fov, float ratio, float nearP)
{
	// convert fov to radians
    float f = 1.0f / tan (fov * Constants::TwoPIOver360 *.5);

    matrix.SetIdentity();

    float data[16];
    memset(data,0,16 * sizeof(float));

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
void Transform::BuildLookAtMatrix(Matrix4x4& matrix, float posX, float posY, float posZ,
								  float lookAtX, float lookAtY, float lookAtZ)
{
	Vector3 vDir(lookAtX - posX, lookAtY - posY, lookAtZ - posZ);
	Vector3 vUp(0,1,0);
	Vector3 vRight;

	vDir.Normalize();

	Vector3::Cross(vDir, vUp, vRight);
	vRight.Normalize();

	Vector3::Cross(vRight, vDir, vUp);
	vUp.Normalize();

	float view[16];

	view[0]  = vRight.x;
	view[4]  = vRight.y;
	view[8]  = vRight.z;
	view[12] = 0.0f;

	view[1]  = vUp.x;
	view[5]  = vUp.y;
	view[9]  = vUp.z;
	view[13] = 0.0f;

	view[2]  = -vDir.x;
	view[6]  = -vDir.y;
	view[10] = -vDir.z;
	view[14] =  0.0f;

	view[3]  = 0.0f;
	view[7]  = 0.0f;
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
 * Shortcut to transform [vector] by [matrix].
 */
void Transform::TransformVector(Vector3& vector) const
{
	matrix.Transform(vector);
}

/*
 * Shortcut to transform [point] by [matrix]
 */
void Transform::TransformPoint(Point3& point) const
{
	matrix.Transform(point);
}

/*
 * Shortcut to transform [vector] by [matrix]
 */
void Transform::TransformVector4f(float * vector) const
{
	ASSERT_RTRN(vector != NULL, "Transform::TransformVector4f -> NULL vector passed.");
	matrix.Transform(vector);
}
