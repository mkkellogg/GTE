#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "point3.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "geometry/vector/vector3.h"

namespace GTE
{
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
	Point3::Point3(Bool permAttached, Real * target) : BaseVector4(permAttached, target),  x(data[0]), y(data[1]), z(data[2])
	{
		data[3] = 1;
	}

	/*
	 * Constructor with initialization values
	 */
	Point3::Point3(Real x, Real y, Real z) : BaseVector4(x,y,z,1), x(data[0]), y(data[1]), z(data[2])
	{

	}

	/*
	 * Copy constructor
	 */
	Point3::Point3(const Point3& point) : BaseVector4(point), x(data[0]), y(data[1]), z(data[2])
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
	void Point3::Add(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	/*
	 * Add vector [v] to [point] and store the result in [result]
	 */
	void Point3::Add(const Point3& point, const Vector3& v, Point3& result)
	{
		result.x = point.x + v.x;
		result.y = point.y + v.y;
		result.z = point.z + v.z;
	}

	/*
	 * Subtract [p2] from [p1] and store the resulting vector in [result]
	 */
	void Point3::Subtract(const Point3& p1,const Point3& p2, Vector3& result)
	{
		result.x = p1.x - p2.x;
		result.y = p1.y - p2.y;
		result.z = p1.z - p2.z;
	}

	/*
	 * Linearly interpolate from [p1] to [2] and store the result in [result]
	 */
	void Point3::Lerp(const Point3& p1, const Point3& p2, Point3& result, Real t)
	{
		result.x = ((p2.x - p1.x) * t) + p1.x;
		result.y = ((p2.y - p1.y) * t) + p1.y;
		result.z = ((p2.z - p1.z) * t) + p1.z;
	}

	/*
	 * Attach to new backing storage in [data]
	 */
	void Point3::AttachTo(Real * data)
	{
		NONFATAL_ASSERT(data != NULL, "Point3::AttachTo -> 'data' is null.", true);
		BaseVector4::AttachTo(data);
	}

	/*
	 * Detach from existing backing storage
	 */
	void Point3::Detach()
	{
		BaseVector4::Detach();
	}

	/*
	 * Assignment operator
	 */
	Point3& Point3::operator= (const Point3& source)
	{
		if(this == &source)return *this;
		BaseVector4::operator=(source);
		return *this;
	}

	/*
	 * Comparison operator
	 */
	Bool Point3::operator==(const Point3 & source)
	{
		Real epsilon = .005f;
		return GTEMath::Abs(source.x - x) < epsilon && GTEMath::Abs(source.y - y) < epsilon && GTEMath::Abs(source.z - z) < epsilon;
	}

	/*
	 * Comparison operator
	 */
	Bool Point3::operator==(const Point3& p) const
	{
		Real epsilon = .005f;
		return GTEMath::Abs(p.x - this->x) < epsilon && GTEMath::Abs(p.y - this->y) < epsilon && GTEMath::Abs(p.z - this->z) < epsilon;
	}

	/*
	 * Comparison function for references
	 */
	Bool Point3::AreEqual(const Point3& a, const Point3& b)
	{
		return AreEqual(&a, &b);
	}

	/*
	 * Comparison function for pointers
	 */
	Bool Point3::AreEqual(const Point3* a, const Point3* b)
	{
		NONFATAL_ASSERT_RTRN(a != NULL && b != NULL, "Point3::AreEqual -> Null point passed.", false, true);

		Real epsilon = .0005f;
		return GTEMath::Abs(a->x - b->x) < epsilon && GTEMath::Abs(a->y - b->y) < epsilon && GTEMath::Abs(a->z - b->z) < epsilon;
	}

	/*
	 * Test for exact equality
	 */
	Bool Point3::AreStrictlyEqual(const Point3* a, const Point3* b)
	{
		NONFATAL_ASSERT_RTRN(a != NULL && b != NULL, "Point3::AreStrictlyEqual -> Null point passed.", false, true);

		return a->x == b->x && a->y == b->y && a->z == b->z;
	}

	/*
	 * Set the values of this point
	 */
	void Point3::Set(Real x, Real y, Real z)
	{
		BaseVector4::Set(x,y,z,1);
	}
}
