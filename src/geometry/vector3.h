#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "basevector4.h"

class Vector3 : public BaseVector4
{
	protected:

	void UpdateComponentPointers();

    public:

    float &x;
    float &y;
    float &z;

    Vector3();
    Vector3(float x, float y, float z);
    Vector3(const Vector3 * vector);
    Vector3(const float * data);
    ~Vector3();

    void Add(const Vector3 * v);
    static void Add(const Vector3 * v1, const Vector3 * v2, Vector3 * result);
    static void Subtract(const Vector3 * v1, const Vector3 * p2, Vector3 * result);

    void Scale(float magnitude);
    void Normalize();
    void QuickNormlize();
    float Magnitude();
    static float Magnitude(float x, float y, float z);
    float QuickMagnitude();
    void Invert();
    static void Cross(const Vector3 * a, const Vector3 * b, Vector3 * results);
    static void CalcNormal(const Vector3 * a,const Vector3 * b, Vector3 * result);
    static float Dot(const Vector3 * a,const Vector3 * b);
    static float AngleBetween(const Vector3 * a,const Vector3 * b,const Vector3 * refRightNormal);

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
