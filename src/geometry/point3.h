#ifndef _POINT3_H_
#define _POINT3_H_

#include "vector3.h"
#include "basevector3.h"

class Point3 : public BaseVector3
{
    public:

    Point3();
    Point3(float x, float y, float z);
    Point3(Point3 * point);
    ~Point3();

    void Add(Vector3 * v);
    static void Add(Point3 * point, Vector3 * v, Point3 * result);
    static void Subtract(Point3 * p1, Point3 * p2, Vector3 * result);
};

#endif
