#ifndef _GTE_QUATERNION_H
#define _GTE_QUATERNION_H

#include <math.h>

#include "engine.h"
#include "geometry/vector/vector3.h"
#include "global/constants.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class Matrix4x4;
	class BaseVector4;

	typedef UInt32 uint32_t;

	class Quaternion
	{
		Real mData[4];

	public:

		static const Quaternion Identity;

		Quaternion();
		Quaternion(const Matrix4x4& matrix);
		Quaternion(const Vector3& v, Real w);
		Quaternion(const BaseVector4& v);
		Quaternion(const Real* array);
		Quaternion(Real x, Real y, Real z, Real w);
		void Set(Real x, Real y, Real z, Real w);
		Real x() const;
		Real y() const;
		Real z() const;
		Real w() const;
		Vector3 complex() const;
		void complex(const Vector3& c);
		Real real() const;
		void real(Real r);
		Quaternion conjugate(void) const;
		Quaternion inverse(void) const;
		Quaternion product(const Quaternion& rhs) const;
		Quaternion operator*(const Quaternion& rhs) const;
		Quaternion operator*(Real s) const;
		Quaternion operator+(const Quaternion& rhs) const;
		Quaternion operator-(const Quaternion& rhs) const;
		Quaternion operator-() const;
		Quaternion operator/(Real s) const;
		Matrix4x4 matrix() const;
		Matrix4x4 rightMatrix() const;
		// Vector3 vector() const;
		void normalize();
		Real norm() const;
		Matrix4x4 rotationMatrix() const;
		void rotationMatrix(Matrix4x4& out) const;
		void FromMatrix(const Matrix4x4& matrix);
		void FromAngleAxis(const Real rfAngle, const Vector3& rkAxis);
		// Vector3 scaledAxis(void) const;
		// void scaledAxis(const Vector3& w);
		// Vector3 rotatedVector(const Vector3& v) const;
		void euler(const Vector3& euler);
		Vector3 euler(void) const;
		//void decoupleZ(Quaternion* Qxy, Quaternion* Qz) const;
		Quaternion slerp(const Quaternion& q1, Real t);
		static Quaternion slerp(const Quaternion& Qa, const Quaternion& Qb, const Real t);
		Real* row(uint32_t i);
		const Real* row(uint32_t i) const;
		static Quaternion getRotation(const Vector3& source, const Vector3& dest);
		static Quaternion getRotation(const Vector3& source, const Vector3& dest, const Vector3& fallbackAxis);
	};

	/**
	 * @brief Global operator allowing left-multiply by scalar.
	 */
	Quaternion operator*(Real s, const Quaternion& q);
}

#endif

