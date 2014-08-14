#ifndef _POINT3_H_
#define _POINT3_H_

#include "vector3.h"
#include "base/basevector4.h"

class Point3 : public BaseVector4
{
	protected:

	void UpdateComponentPointers();

    public:

    float &x;
    float &y;
    float &z;

    Point3();
    Point3(bool permAttached, float * target);
    Point3(float x, float y, float z);
    Point3(const Point3 * point);
    Point3(const float * data);
    ~Point3();

    void Add(const Vector3 * v);
    static void Add(const Point3 * point, const Vector3 * v, Point3 * result);
    static void Subtract(const Point3 * p1,const Point3 * p2, Vector3 * result);

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
