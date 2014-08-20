#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "point3.h"
#include "geometry/vector/vector3.h"

Point3::Point3() : BaseVector4(), x(data[0]), y(data[1]), z(data[2])
{

}

Point3::Point3(bool permAttached, float * target) : BaseVector4(permAttached, target),  x(data[0]), y(data[1]), z(data[2])
{

}

Point3::Point3(float x, float y, float z) : BaseVector4(x,y,z,1), x(data[0]), y(data[1]), z(data[2])
{

}

Point3::Point3(const Point3 * point) : BaseVector4(point), x(data[0]), y(data[1]), z(data[2])
{

}

Point3::~Point3()
{
    
}

void Point3::Add(const Vector3 * v)
{
    x += v->x;
    y += v->y;
    z += v->z;
}

void Point3::Add(const Point3 * point,const Vector3 * v, Point3 * result)
{
    result->x = point->x + v->x;
    result->y = point->y + v->y;
    result->z = point->z + v->z;
}

void Point3::Subtract(const Point3 * p1,const Point3 * p2, Vector3 * result)
{
    result->x = p1->x - p2->x;
    result->y = p1->y - p2->y;        
    result->z = p1->z - p2->z;
}

void Point3::AttachTo(float * data)
{
	BaseVector4::AttachTo(data);
	UpdateComponentPointers();
}

void Point3::Detach()
{
	BaseVector4::Detach();
	UpdateComponentPointers();
}

void Point3::UpdateComponentPointers()
{
    float ** rPtr;
    rPtr = (float **)&x;
    *rPtr = data;
    rPtr = (float **)&y;
    *rPtr = data+1;
    rPtr = (float **)&z;
    *rPtr = data+2;
}

void Point3::Set(float x, float y, float z)
{
	BaseVector4::Set(x,y,z,1);
}

