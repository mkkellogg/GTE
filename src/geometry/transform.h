#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

// forward declarations
class Matrix4x4;
class Transform;

class Transform;

class Transform
{
	Matrix4x4 * matrix;

    public:

    Transform();
    Transform(Matrix4x4 * m);
    ~Transform();

    const  Matrix4x4 * GetMatrix() const ;

    static Transform * CreateIdentityTransform();

    static void BuildProjectionMatrix(Matrix4x4 * m,float fov, float ratio, float nearP, float farP);
    static void BuildLookAtMatrix(Matrix4x4 * m, float posX, float posY, float posZ,
            						float lookAtX, float lookAtY, float lookAtZ);
};

#endif
