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

Transform * Transform::CreateIdentityTransform()
{
	return new Transform();
}

Transform::Transform()
{
	matrix.SetIdentity();
}

Transform::Transform(Matrix4x4 * m) : Transform()
{
	matrix.SetTo(m);
}

Transform::Transform(Transform * transform) : Transform()
{
	if(transform != NULL)
	{
		matrix.SetTo(transform->GetMatrix());
	}
}

Transform::~Transform()
{

}

const Matrix4x4 * Transform::GetMatrix() const
{
	return &matrix;
}

void Transform::SetTo(const Matrix4x4 * matrix)
{
	this->matrix.SetTo(matrix);
}

void Transform::SetTo(const Transform * transform)
{
	SetTo(transform->GetMatrix());
}

void Transform::SetIdentity()
{
	matrix.SetIdentity();
}

void Transform::TransformBy(const Transform * transform)
{
	matrix.Multiply(transform->GetMatrix());
}

void Transform::PreTransformBy(const Transform * transform)
{
	matrix.PreMultiply(transform->GetMatrix());
}


void Transform::Invert()
{
	matrix.Invert();
}

void Transform::Translate(float x, float y, float z, bool local)
{
	if(!local)
	{
		float trans[] = {x,y,z,0};
		Transform full;
		full.SetTo(this);
		full.Invert();
		full.GetMatrix()->Transform(trans);
		matrix.Translate(trans[0], trans[1], trans[2]);
	}
	else matrix.Translate(x,y,z);
}

void Transform::RotateAround(Point3 * point, Vector3 * axis, float angle)
{
	RotateAround(point->x, point->y, point->z, axis->x, axis->y, axis->z, angle);
}

void Transform::RotateAround(float px, float py, float pz, float ax, float ay, float az, float angle)
{
	/*matrix.PreTranslate(-px,-py,-pz);
	matrix.PreRotate(ax,ay,az,angle);
	matrix.PreTranslate(px,py,pz);*/

	float pointTrans[] = {px,py,pz,1};
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
	matrix.Translate(-diffX,-diffY,-diffZ);
}

void Transform::BuildProjectionMatrix(Matrix4x4 * m,float fov, float ratio, float nearP, float farP)
{
    float f = 1.0f / tan (fov * Constants::PIOver360);

    m->SetIdentity();

    float data[16];
    memset(data,0,16 * sizeof(float));

    data[0] = f / ratio;
    data[1 * 4 + 1] = f;
    data[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    data[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    data[2 * 4 + 3] = -1.0f;
    data[3 * 4 + 3] = 0.0f;

    m->SetTo(data);
}

void Transform::BuildLookAtMatrix(Matrix4x4 * m, float posX, float posY, float posZ,
								  float lookAtX, float lookAtY, float lookAtZ)
{
	Vector3 vDir(lookAtX - posX, lookAtY - posY, lookAtZ - posZ);
	Vector3 vUp(0,1,0);
	Vector3 vRight;

	vDir.Normalize();

	Vector3::Cross((const Vector3 *)&vDir, (const Vector3 *)&vUp, &vRight);
	vRight.Normalize();

	Vector3::Cross((const Vector3 *)&vRight, (const Vector3 *)&vDir, &vUp);
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

	Matrix4x4::Multiply(&viewMatrix, &aux, m);
}
