#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "basevector3.h"

class Vector3 : public BaseVector3
{
    public:

    Vector3();
    Vector3(float x, float y, float z);
    Vector3(Vector3 * vector);
    ~Vector3();

    void Add(Vector3 * v);
    static void Add(Vector3 * v1, Vector3 * v2, Vector3 * result);
    static void Subtract(Vector3 * v1, Vector3 * p2, Vector3 * result);
};

#endif
