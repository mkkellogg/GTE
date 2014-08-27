#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

// forward declarations
class Transform;

#include "matrix4x4.h"
#include "util/datastack.h"

class Transform
{
	Matrix4x4 matrix;
	DataStack<float> *matrixStack;
	bool matrixStackInitialized;

    public:

    Transform();
    Transform(Matrix4x4 * m);
    Transform(Transform *);
    ~Transform();

    const  Matrix4x4 * GetMatrix() const ;
    void SetTo(const Matrix4x4 * matrix);

    void Invert();
    void TransformBy(const Transform * transform, bool push);
    void Translate(float x, float y, float z, bool local);
    void RotateAround(Point3 * point, Vector3 * axis, float angle);
    void RotateAround(float px, float py, float pz, float ax, float ay, float az,  float angle);

    static Transform * CreateIdentityTransform();

    static void BuildProjectionMatrix(Matrix4x4 * m,float fov, float ratio, float nearP, float farP);
    static void BuildLookAtMatrix(Matrix4x4 * m, float posX, float posY, float posZ,
            						float lookAtX, float lookAtY, float lookAtZ);
};

#endif
