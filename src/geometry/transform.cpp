#include <iostream>
#include <math.h>
#include "transform.h"
#include "matrix.h"
#include "ui/debug.h"
#include "global/constants.h"
#include "vector/vector3.h"


Transform * Transform::CreteIdentityTransform()
{
	return new Transform();
}

Transform::Transform()
{
	matrix = new Matrix();
	matrix->SetIdentity();
}

Transform::Transform(Matrix * m) : Transform()
{
	matrix->SetTo(m);
}

Transform::~Transform()
{
	delete matrix;
}

const Matrix * Transform::GetMatrix() const
{
	return (const Matrix *)matrix;
}

void Transform::BuildProjectionMatrix(Matrix * m,float fov, float ratio, float nearP, float farP)
{
    float f = 1.0f / tan (fov * Constants::PIOver360);

    m->SetIdentity();

    float * data = m->GetDataPtr();

    data[0] = f / ratio;
    data[1 * 4 + 1] = f;
    data[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
    data[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
    data[2 * 4 + 3] = -1.0f;
    data[3 * 4 + 3] = 0.0f;
}

void Transform::BuildLookAtMatrix(Matrix * m, float posX, float posY, float posZ,
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

	Matrix viewMatrix;
	float * view = viewMatrix.GetDataPtr();

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

	Matrix aux;

	aux.SetIdentity();
	aux.Translate(-posX, -posY, -posZ);

	Matrix::Multiply(&viewMatrix, &aux, m);
}
