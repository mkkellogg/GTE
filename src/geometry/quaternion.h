#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>
#include <math.h>
#include "matrix.h"
#include "vector/vector3.h"

class Quaternion 
{
    float mData[4];
  
    public:
  
    Quaternion();
    Quaternion(const Vector3& v, float w) ;
    Quaternion(const BaseVector4& v);
    Quaternion(const float* array);
    Quaternion(float x, float y, float z, float w);
    float x() const;
    float y() const;
    float z() const;
    float w() const;
    Vector3 complex() const;
    void complex(const Vector3& c);
    float real() const;
    void real(float r);
    Quaternion conjugate(void) const;
    Quaternion inverse(void) const;
    Quaternion product(const Quaternion& rhs) const;
    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion operator*(float s) const;
    Quaternion operator+(const Quaternion& rhs) const;
    Quaternion operator-(const Quaternion& rhs) const;
    Quaternion operator-() const;
    Quaternion operator/(float s) const;
    Matrix matrix() const;
    Matrix rightMatrix() const;
    // Vector3 vector() const;
    float norm() const;
    // Matrix rotationMatrix() const;
    // Vector3 scaledAxis(void) const;
    // void scaledAxis(const Vector3& w);
    // Vector3 rotatedVector(const Vector3& v) const;
    void euler(const Vector3& euler);
    Vector3 euler(void) const;
    //void decoupleZ(Quaternion* Qxy, Quaternion* Qz) const;
    Quaternion slerp(const Quaternion& q1, float t);
    static Quaternion slerp(const Quaternion& Qa, const Quaternion& Qb, const float t);
    float* row(uint32_t i);
    const float* row(uint32_t i) const;
};

/**
 * @brief Global operator allowing left-multiply by scalar.
 */
Quaternion operator*(float s, const Quaternion& q);

#endif

