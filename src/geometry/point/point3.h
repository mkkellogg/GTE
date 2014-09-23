/*
 * class: Point3
 *
 * author: Mark Kellogg
 *
 * A utility class for performing 3D point operations.
 *
 */

#ifndef _POINT3_H_
#define _POINT3_H_

//forward declarations
class Vector3;
class Point3;

#include "base/basevector4.h"
#include <functional>
#include <unordered_map>
#include <iostream>
#include <string>


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
    Point3(const Point3& point);
    Point3(const Point3 * point);
    Point3(const float * data);
    ~Point3();

    void Add(const Vector3 * v);
    static void Add(const Point3 * point, const Vector3 * v, Point3 * result);
    static void Subtract(const Point3 * p1,const Point3 * p2, Vector3 * result);

    Point3 & operator= (const Point3 & source);

    void Set(float x, float y, float z);

    void AttachTo(float * data);
    void Detach();

    typedef struct
    {
    	 int operator()(const Point3& p) const
    	 {
			  return (int)p.x;
    	 }
    }Point3Hasher;

    typedef struct
    {
      bool operator() (const Point3& a, const Point3& b) const { return a==b; }
    } Point3Eq;

    bool operator==(const Point3& p) const
    {
		return p.x == this->x && p.y == this->y && p.z == this->z;
    }
};

#endif
