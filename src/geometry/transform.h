/*
 * class: Transform
 *
 * author: Mark Kellogg
 *
 * A Transform object encapsulates a Matrix4x4 object and provides convenience
 * functions and wrappers for it.
 */

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

// forward declarations
class Transform;

#include "matrix4x4.h"
#include "util/datastack.h"

class Transform
{
	// give access to the transform's matrix
	friend class RenderManager;

	protected:

	Matrix4x4 matrix;

    public:

    Transform();
    Transform(const Matrix4x4& m);
    Transform(Transform *);
    virtual ~Transform();

    void CopyMatrix(Matrix4x4& dest) const;
    void SetTo(const Matrix4x4& matrix);
    void SetTo(const Transform& transform);
    void SetTo(const float * matrixData);
    void SetIdentity();
    void Invert();
    void TransformBy(const Matrix4x4& matrix);
    void PreTransformBy(const Matrix4x4& matrix);
    void TransformBy(const Transform& transform);
    void PreTransformBy(const Transform& transform);


    virtual void Translate(float x, float y, float z, bool local);
    virtual void Translate(Vector3& vector, bool local);
    virtual void RotateAround(Point3 * point, Vector3 * axis, float angle);
    virtual void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle);
    virtual void Scale(Vector3 * mag,  bool local);
    virtual void Scale(float x, float y, float z,  bool local);
    virtual void Rotate(const Vector3& vector, float a);
    virtual void Rotate(float x, float y, float z, float a);

    void TransformVector(Vector3& vector) const;
    void TransformPoint(Point3& point3) const;
    void TransformVector4f(float * vector) const;
    static Transform * CreateIdentityTransform();

    static void BuildProjectionMatrix(Matrix4x4& matrix, float fov, float ratio, float nearP, float farP);
    static void BuildProjectionMatrixInfiniteFar(Matrix4x4& matrix, float fov, float ratio, float nearP);
    static void BuildLookAtMatrix(Matrix4x4& matrix, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ);
};

#endif
