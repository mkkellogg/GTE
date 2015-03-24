/*
 * class: Vector3
 *
 * author: Mark Kellogg
 *
 * A utility class for performing 3D vector operations.
 *
 */

#ifndef _GTE_VECTOR3_H_
#define _GTE_VECTOR3_H_

#include "base/basevector4.h"

class Vector3 : public BaseVector4
{
	protected:

	void UpdateComponentPointers();

    public:

	static const Vector3 Zero;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 Up;
	static const Vector3 Forward;

    float &x;
    float &y;
    float &z;

    Vector3();
    Vector3(bool permAttached, float * target);
    Vector3(float x, float y, float z);
    Vector3(const Vector3& vector);
    Vector3(const float * data);
    ~Vector3();

    Vector3& operator=(const Vector3 & source);
    BaseVector4 & operator= (const BaseVector4 & source);
    bool operator==(const Vector3 & source) const;
    bool operator==(const Vector3 & source);
    static bool AreStrictlyEqual(const Vector3* a, const Vector3* b);
    void Set(float x, float y, float z);

    void Add(const Vector3& v);
    static void Add(const Vector3& v1, const Vector3& v2, Vector3& result);
    static void Subtract(const Vector3& v1, const Vector3& p2, Vector3& result);

    void Scale(float magnitude);
    void Normalize();
    void QuickNormalize();
    float Magnitude() const ;
    float SquareMagnitude() const ;
    static float Magnitude(float x, float y, float z);
    static float SquareMagnitude(float x, float y, float z);
    float QuickMagnitude() const;
    void Invert();
    static void Cross(const Vector3& a, const Vector3& b, Vector3& results);
    static void CalcNormal(const Vector3& a,const Vector3& b, Vector3& result);
    static float Dot(const Vector3& a,const Vector3& b);
    static bool RotateTowards(const Vector3& from,const Vector3& to,  float theta, Vector3& result);
    static bool RotateTowards(const Vector3& from, const Vector3& to,  float theta, Vector3& result, const Vector3& fallbackAxis);
    static float AngleBetween(const Vector3 * a,const Vector3 * b,const Vector3 * refRightNormal);

    bool IsZeroLength() const;

    void AttachTo(float * data);
    void Detach();
};

#endif
