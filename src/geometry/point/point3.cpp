#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "ui/debug.h"
#include "global/global.h"
#include "point3.h"
#include "geometry/vector/vector3.h"

/*
 * Default constructor
 */
Point3::Point3() : BaseVector4(), x(data[0]), y(data[1]), z(data[2])
{
	data[3] = 1;
}

/*
 * Constructor will alternate backing storage
 */
Point3::Point3(bool permAttached, float * target) : BaseVector4(permAttached, target),  x(data[0]), y(data[1]), z(data[2])
{
	data[3] = 1;
}

/*
 * Constructor with initialization values
 */
Point3::Point3(float x, float y, float z) : BaseVector4(x,y,z,1), x(data[0]), y(data[1]), z(data[2])
{

}

/*
 * Copy constructor
 */
Point3::Point3(const Point3& point) : BaseVector4(&point), x(data[0]), y(data[1]), z(data[2])
{

}

/*
 * Copy constructor
 */
Point3::Point3(const Point3 * point) : BaseVector4(point), x(data[0]), y(data[1]), z(data[2])
{

}

/*
 * Clean up
 */
Point3::~Point3()
{
    
}

/*
 * Move this point by [v]
 */
void Point3::Add(const Vector3 * v)
{
    x += v->x;
    y += v->y;
    z += v->z;
}

/*
 * Add vector [v] to [point] and store the result in [result]
 */
void Point3::Add(const Point3 * point,const Vector3 * v, Point3 * result)
{
    result->x = point->x + v->x;
    result->y = point->y + v->y;
    result->z = point->z + v->z;
}

/*
 * Subtract [p2] from [p1] and store the resulting vector in [result]
 */
void Point3::Subtract(const Point3 * p1,const Point3 * p2, Vector3 * result)
{
	NULL_CHECK_RTRN(p1, "Point3::Subtract -> NULL p1 passed.");
	NULL_CHECK_RTRN(p2, "Point3::Subtract -> NULL p2 passed.");
	NULL_CHECK_RTRN(result, "Point3::Subtract -> NULL result passed.");

    result->x = p1->x - p2->x;
    result->y = p1->y - p2->y;        
    result->z = p1->z - p2->z;
}

/*
 * Linearly interpolate from [p1] to [2] and store the result in [result]
 */
void Point3::Lerp(const Point3 *p1, const Point3 * p2, Point3 * result, float t)
{
	NULL_CHECK_RTRN(p1, "Point3::Lerp -> NULL p1 passed.");
	NULL_CHECK_RTRN(p2, "Point3::Lerp -> NULL p2 passed.");
	NULL_CHECK_RTRN(result, "Point3::Lerp -> NULL result passed.");

	result->x = ((p2->x - p1->x) * t) + p1->x;
	result->y = ((p2->y - p1->y) * t) + p1->y;
	result->z = ((p2->z - p1->z) * t) + p1->z;
}

/*
 * Attach to new backing storage in [data]
 */
void Point3::AttachTo(float * data)
{
	NULL_CHECK_RTRN(data, "Point3::AttachTo -> NULL data passed.");

	BaseVector4::AttachTo(data);
	UpdateComponentPointers();
}

/*
 * Detach from existing backing storage
 */
void Point3::Detach()
{
	BaseVector4::Detach();
	UpdateComponentPointers();
}

/*
 * Update the element accessor variables to point to the current backing storage. This is
 * usually called when the point is attached to new backing storage.
 */
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

/*
 * Assignment operator
 */
Point3 & Point3::operator= (const Point3 & source)
{
    if(this == &source)return *this;
    memcpy((void*)data, (void*)source.data, sizeof(float) * 4);
    memcpy((void*)baseData, (void*)source.baseData, sizeof(float) * 4);
    return *this;
}


/*
 * Set the values of this point
 */
void Point3::Set(float x, float y, float z)
{
	BaseVector4::Set(x,y,z,1);
}

void Point3::SetTo(Point3 * p)
{
	NULL_CHECK_RTRN(p, "Point3::SetTo -> NULL point passed.");

	BaseVector4::Set(p->x,p->y,p->z,1);
}

