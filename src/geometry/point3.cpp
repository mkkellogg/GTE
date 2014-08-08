#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "point3.h"
#include "vector3.h"

Point3::Point3() : BaseVector3()
{

}

Point3::Point3(float x, float y, float z) : BaseVector3(x,y,z)
{
   
}

Point3::Point3(Point3 * point) : BaseVector3(point)
{
   
}

Point3::~Point3()
{
    
}

void Point3::Add(Vector3 * v)
{
    x += v->x;
    y += v->y;
    z += v->z;
}

void Point3::Add(Point3 * point, Vector3 * v, Point3 * result)
{
    result->x = point->x + v->x;
    result->y = point->y + v->y;
    result->z = point->z + v->z;
}

void Point3::Subtract(Point3 * p1, Point3 * p2, Vector3 * result)
{
    result->x = p1->x - p2->x;
    result->y = p1->y - p2->y;        
    result->z = p1->z - p2->z;
}

