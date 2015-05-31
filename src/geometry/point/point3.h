/*
 * class: Point3
 *
 * author: Mark Kellogg
 *
 * A utility class for performing 3D point operations.
 *
 */

#ifndef _GTE_POINT3_H_
#define _GTE_POINT3_H_

#include "base/basevector4.h"
#include "gtemath/gtemath.h"
#include <functional>
#include <unordered_map>
#include <iostream>
#include <string>

namespace GTE
{
	//forward declarations
	class Vector3;

	class Point3 : public BaseVector4
	{
	protected:

		void UpdateComponentPointers();

	public:

		Real &x;
		Real &y;
		Real &z;

		Point3();
		Point3(bool permAttached, Real * target);
		Point3(Real x, Real y, Real z);
		Point3(const Point3& point);
		Point3(const Real * data);
		~Point3();

		void Add(const Vector3& v);
		static void Add(const Point3& point, const Vector3& v, Point3& result);
		static void Subtract(const Point3& p1, const Point3& p2, Vector3&  result);
		static void Lerp(const Point3& p1, const Point3& p2, Point3& result, Real t);

		Point3 & operator= (const Point3 & source);
		BaseVector4 & operator= (const BaseVector4 & source);
		bool operator==(const Point3 & source);
		bool operator==(const Point3& p) const;
		static bool AreEqual(const Point3& a, const Point3& b);
		static bool AreEqual(const Point3* a, const Point3* b);
		static bool AreStrictlyEqual(const Point3* a, const Point3* b);
		void Set(Real x, Real y, Real z);

		void AttachTo(Real * data);
		void Detach();

		// TODO: optimize this hashing function (implement correctly)
		typedef struct
		{
			Int32 operator()(const Point3& p) const
			{
				return (Int32)p.x + (6 << (Int32)p.y) + (12 << (Int32)p.z);
			}

			Int32 operator()(const Point3& p)
			{
				return (Int32)p.x + (6 << (Int32)p.y) + (12 << (Int32)p.z);
			}
		}Point3Hasher;

		typedef struct
		{
			bool operator() (const Point3& a, const Point3& b) const { return a.x == b.x && a.y == b.y && a.z == b.z; }
		} Point3Eq;
	};
}



#endif
