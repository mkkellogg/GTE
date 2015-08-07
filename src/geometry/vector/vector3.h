/*
 * class: Vector3
 *
 * author: Mark Kellogg
 *
 * A utility class for performing 3D vector operations.
 *
 */

#ifndef _GTE_Vector3_H_
#define _GTE_Vector3_H_

#include "base/basevector4.h"

namespace GTE
{
	class Vector3 : public BaseVector4
	{
	protected:

	public:

		static const Vector3 Zero;
		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
		static const Vector3 Up;
		static const Vector3 Forward;

		Real &x;
		Real &y;
		Real &z;

		Vector3();
		Vector3(Bool permAttached, Real * target);
		Vector3(Real x, Real y, Real z);
		Vector3(const Vector3& vector);
		Vector3(const Real * data);
		~Vector3() override;

		Vector3& operator=(const Vector3 & source);
		Bool operator==(const Vector3 & source) const;
		Bool operator==(const Vector3 & source);
		static Bool AreStrictlyEqual(const Vector3* a, const Vector3* b);
		void Set(Real x, Real y, Real z);

		void Add(const Vector3& v);
		static void Add(const Vector3& v1, const Vector3& v2, Vector3& result);
		static void Subtract(const Vector3& v1, const Vector3& p2, Vector3& result);

		void Scale(Real magnitude);
		void Normalize();
		void QuickNormalize();
		Real Magnitude() const;
		Real SquareMagnitude() const;
		static Real Magnitude(Real x, Real y, Real z);
		static Real SquareMagnitude(Real x, Real y, Real z);
		Real QuickMagnitude() const;
		void Invert();
		static void Cross(const Vector3& a, const Vector3& b, Vector3& results);
		static void CalcNormal(const Vector3& a, const Vector3& b, Vector3& result);
		static Real Dot(const Vector3& a, const Vector3& b);
		static Bool RotateTowards(const Vector3& from, const Vector3& to, Real theta, Vector3& result);
		static Bool RotateTowards(const Vector3& from, const Vector3& to, Real theta, Vector3& result, const Vector3& fallbackAxis);
		static Real AngleBetween(const Vector3 * a, const Vector3 * b, const Vector3 * refRightNormal);

		Bool IsZeroLength() const;

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};
}

#endif
