#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>
#include <math.h>
#include "matrix.h"
#include "vector3.h"

class Quaternion 
{
    double mData[4];
  
    public:
  
    Quaternion();
    Quaternion(const Vector3& v, double w) ;
    Quaternion(const Vector3& v);
    Quaternion(const double* array);
    Quaternion(double x, double y, double z, double w);
    double x() const;
    double y() const;
    double z() const;
    double w() const;
    Vector3 complex() const;
    void complex(const Vector3& c);
    double real() const;
    void real(double r);
    Quaternion conjugate(void);
    Quaternion inverse(void) const;
    Quaternion product(const Quaternion& rhs) const;
    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion operator*(double s) const;
    Quaternion operator+(const Quaternion& rhs) const;
    Quaternion operator-(const Quaternion& rhs) const;
    Quaternion operator-() const;
    Quaternion operator/(double s) const;
    Matrix matrix() const;
    Matrix rightMatrix() const;
    Matrix vector() const;
    double norm() const;
    Matrix rotationMatrix() const;
    Vector3 scaledAxis(void) const;
    void scaledAxis(const Vector3& w);
    Vector3 rotatedVector(const Vector3& v) const;
    void euler(const Vector3& euler);
    Vector3 euler(void) const;
    void decoupleZ(Quaternion* Qxy, Quaternion* Qz) const;
    Quaternion slerp(const Quaternion& q1, double t);
    static Quaternion slerp(const Quaternion& q0, const Quaternion& q1, double t);
    double* row(uint32_t i);
    const double* row(uint32_t i) const;
};

/**
 * @brief Global operator allowing left-multiply by scalar.
 */
Quaternion operator*(double s, const Quaternion& q);


#endif /* QUATERNION_H */

