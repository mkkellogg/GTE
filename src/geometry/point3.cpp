#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "point3.h"
#include "vector3.h"

Point3::Point3() : BaseVector4(), x(data[0]), y(data[1]), z(data[2])
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

