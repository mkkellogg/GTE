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
	protected:

	Matrix4x4 matrix;

    public:

    Transform();
    Transform(Matrix4x4 * m);
    Transform(Transform *);
    virtual ~Transform();

    const  Matrix4x4 * GetMatrix() const ;
    void SetTo(const Matrix4x4 * matrix);
    void SetTo(const Transform * transform);
    void SetTo(const Transform& transform);

    void SetIdentity();
    void Invert();
    void TransformBy(const Transform * transform);
    void PreTransformBy(const Transform * transform);
    void TransformBy(const Transform& transform);
    void PreTransformBy(const Transform& transform);
    virtual void Translate(float x, float y, float z, bool local);
    virtual void RotateAround(Point3 * point, Vector3 * axis, float angle);
    virtual void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle);
    virtual void Scale(Vector3 * mag,  bool local);
    virtual void Scale(float x, float y, float z,  bool local);

    static Transform * CreateIdentityTransform();

    static void BuildProjectionMatrix(Matrix4x4 * m,float fov, float ratio, float nearP, float farP);
    static void BuildLookAtMatrix(Matrix4x4 * m, float posX, float posY, float posZ,
            						float lookAtX, float lookAtY, float lookAtZ);
};

#endif
