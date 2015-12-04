#include <math.h>
#include <cmath>

#include "quaternion.h"
#include "matrix4x4.h"
#include "gtemath/gtemath.h"
#include "vector/vector3.h"
#include "base/basevector.h"
#include "debug/gtedebug.h"
#include "global/constants.h"

namespace GTE
{
	const Quaternion Quaternion::Identity(0, 0, 0, 1);

	Quaternion::Quaternion()
	{
		mData[0] = mData[1] = mData[2] = 0;
		mData[3] = 1;
	}

	Quaternion::Quaternion(const Vector3& v, Real w)
	{
		mData[0] = v.x;
		mData[1] = v.y;
		mData[2] = v.z;
		mData[3] = w;
	}

	Quaternion::Quaternion(const Real* array)
	{
		/*if (!array)
		 {
		 UAV_EXCEPTION("Constructing quaternion from 0 array.");
		 }*/
		for (uint32_t i = 0; i < 4; i++)
		{
			mData[i] = array[i];
		}
	}

	Quaternion::Quaternion(Real x, Real y, Real z, Real w)
	{
		mData[0] = x;
		mData[1] = y;
		mData[2] = z;
		mData[3] = w;
	}

	void Quaternion::Set(Real x, Real y, Real z, Real w)
	{
		mData[0] = x;
		mData[1] = y;
		mData[2] = z;
		mData[3] = w;
	}

	Quaternion::Quaternion(const Matrix4x4& matrix)
	{
		FromMatrix(matrix);
	}


	Real Quaternion::x() const
	{
		return mData[0];
	}
	Real Quaternion::y() const
	{
		return mData[1];
	}
	Real Quaternion::z() const
	{
		return mData[2];
	}
	Real Quaternion::w() const
	{
		return real();
	}

	Vector3 Quaternion::complex() const
	{
		return Vector3(mData[0], mData[1], mData[2]);
	}

	void Quaternion::complex(const Vector3& c)
	{
		mData[0] = c.x;
		mData[1] = c.y;
		mData[2] = c.z;
	}

	Real Quaternion::real() const
	{
		return mData[3];
	}

	void Quaternion::real(Real r)
	{
		mData[3] = r;
	}

	Quaternion Quaternion::conjugate(void) const
	{
		Vector3 comp = complex();
		comp.x = -comp.x;
		comp.y = -comp.y;
		comp.z = -comp.z;
		return Quaternion(comp, real());
	}

	/**
	 * @brief Computes the inverse of this quaternion.
	 *
	 * @note This is a general inverse.  If you know a priori
	 * that you're using a unit quaternion (i.e., norm() == 1),
	 * it will be significantly faster to use conjugate() instead.
	 *
	 * @return The quaternion q such that q * (*this) == (*this) * q
	 * == [ 0 0 0 1 ]<sup>T</sup>.
	 */
	Quaternion Quaternion::inverse(void) const
	{
		return conjugate() / norm();
	}

	/**
	 * @brief Computes the product of this quaternion with the
	 * quaternion 'rhs'.
	 *
	 * @param rhs The right-hand-side of the product operation.
	 *
	 * @return The quaternion product (*this) x @p rhs.
	 */
	Quaternion Quaternion::product(const Quaternion& rhs) const
	{
		return Quaternion(y() * rhs.z() - z() * rhs.y() + x() * rhs.w() + w() * rhs.x(),
			z() * rhs.x() - x() * rhs.z() + y() * rhs.w() + w() * rhs.y(),
			x() * rhs.y() - y() * rhs.x() + z() * rhs.w() + w() * rhs.z(),
			w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z());
	}

	/**
	 * @brief Quaternion product operator.
	 *
	 * The result is a quaternion such that:
	 *
	 * result.real() = (*this).real() * rhs.real() -
	 * (*this).complex().dot(rhs.complex());
	 *
	 * and:
	 *
	 * result.complex() = rhs.complex() * (*this).real
	 * + (*this).complex() * rhs.real()
	 * - (*this).complex().cross(rhs.complex());
	 *
	 * @return The quaternion product (*this) x rhs.
	 */
	Quaternion Quaternion::operator*(const Quaternion& rhs) const
	{
		return product(rhs);
	}

	/**
	 * @brief Quaternion scalar product operator.
	 * @param s A scalar by which to multiply all components
	 * of this quaternion.
	 * @return The quaternion (*this) * s.
	 */
	Quaternion Quaternion::operator*(Real s) const
	{
		Vector3 comp = complex();
		comp.x *= s;
		comp.y *= s;
		comp.z *= s;
		return Quaternion(comp, real() * s);
	}

	/**
	 * @brief Produces the sum of this quaternion and rhs.
	 */
	Quaternion Quaternion::operator+(const Quaternion& rhs) const
	{
		return Quaternion(x() + rhs.x(), y() + rhs.y(), z() + rhs.z(), w() + rhs.w());
	}

	/**
	 * @brief Produces the difference of this quaternion and rhs.
	 */
	Quaternion Quaternion::operator-(const Quaternion& rhs) const
	{
		return Quaternion(x() - rhs.x(), y() - rhs.y(), z() - rhs.z(), w() - rhs.w());
	}

	/**
	 * @brief Unary negation.
	 */
	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-x(), -y(), -z(), -w());
	}

	/**
	 * @brief Quaternion scalar division operator.
	 * @param s A scalar by which to divide all components
	 * of this quaternion.
	 * @return The quaternion (*this) / s.
	 */
	Quaternion Quaternion::operator/(Real s) const
	{
		if (s == 0)
			Debug::PrintMessage("Dividing quaternion by 0.\n");
		Vector3 comp = complex();
		comp.x /= s;
		comp.y /= s;
		comp.z /= s;
		return Quaternion(comp, real() / s);
	}

	/**
	 * @brief Returns a matrix representation of this
	 * quaternion.
	 *
	 * Specifically this is the matrix such that:
	 *
	 * this->matrix() * q.vector() = (*this) * q for any quaternion q.
	 *
	 * Note that this is @e NOT the rotation matrix that may be
	 * represented by a unit quaternion.
	 */
	Matrix4x4 Quaternion::matrix() const
	{
		Real m[16] = { w(), -z(), y(), x(), z(), w(), -x(), y(), -y(), x(), w(), z(), -x(), -y(), -z(), w() };

		return Matrix4x4(m);
	}

	/**
	 * @brief Returns a matrix representation of this
	 * quaternion for right multiplication.
	 *
	 * Specifically this is the matrix such that:
	 *
	 * q.vector().transpose() * this->matrix() = (q *
	 * (*this)).vector().transpose() for any quaternion q.
	 *
	 * Note that this is @e NOT the rotation matrix that may be
	 * represented by a unit quaternion.
	 */
	Matrix4x4 Quaternion::rightMatrix() const
	{
		Real m[16] = { +w(), -z(), y(), -x(), +z(), w(), -x(), -y(), -y(), x(), w(), -z(), +x(), y(), z(), w() };
		return Matrix4x4(m);
	}

	/**
	 * @brief Returns this quaternion as a 4-vector.
	 *
	 * This is simply the vector [x y z w]<sup>T</sup>
	 */
	//TVector4 vector() const { return TVector4(mData); }

	void Quaternion::normalize()
	{
		Real normFactor = norm();
		mData[0] /= normFactor;
		mData[1] /= normFactor;
		mData[2] /= normFactor;
		mData[3] /= normFactor;
	}

	/**
	 * @brief Returns the norm ("magnitude") of the quaternion.
	 * @return The 2-norm of [ w(), x(), y(), z() ]<sup>T</sup>.
	 */
	Real Quaternion::norm() const
	{
		return GTEMath::SquareRoot(mData[0] * mData[0] + mData[1] * mData[1] + mData[2] * mData[2] + mData[3] * mData[3]);
	}

	/**
	 * @brief Computes the rotation matrix represented by a unit
	 * quaternion.
	 *
	 * @note This does not check that this quaternion is normalized.
	 * It formulaically returns the matrix, which will not be a
	 * rotation if the quaternion is non-unit.
	 */
	Matrix4x4 Quaternion::rotationMatrix() const
	{
		/*Real m[16] =
		{
		1-2*y()*y()-2*z()*z(), 2*x()*y() - 2*z()*w(), 2*x()*z() + 2*y()*w(),0,
		2*x()*y() + 2*z()*w(), 1-2*x()*x()-2*z()*z(), 2*y()*z() - 2*x()*w(),0,
		2*x()*z() - 2*y()*w(), 2*y()*z() + 2*x()*w(), 1-2*x()*x()-2*y()*y(),0,
		0,0,0,1
		};*/

		Real m[16] =
		{
			1 - 2 * y()*y() - 2 * z()*z(), 2 * x()*y() + 2 * z()*w(), 2 * x()*z() - 2 * y()*w(), 0,
			2 * x()*y() - 2 * z()*w(), 1 - 2 * x()*x() - 2 * z()*z(), 2 * y()*z() + 2 * x()*w(), 0,
			2 * x()*z() + 2 * y()*w(), 2 * y()*z() - 2 * x()*w(), 1 - 2 * x()*x() - 2 * y()*y(), 0,
			0, 0, 0, 1
		};
		Matrix4x4 p(m);
		return p;
	}

	void Quaternion::rotationMatrix(Matrix4x4& out) const
	{
		Real * outData = const_cast<Real*>(out.GetDataPtr());
		
		outData[0] = 1 - 2 * y()*y() - 2 * z()*z();
		outData[1] = 2 * x()*y() + 2 * z()*w();
		outData[2] = 2 * x()*z() - 2 * y()*w();
		outData[3] = 0;
		
		outData[4] = 2 * x()*y() - 2 * z()*w();
		outData[5] = 1 - 2 * x()*x() - 2 * z()*z();
		outData[6] = 2 * y()*z() + 2 * x()*w();
		outData[7] = 0;
		
		outData[8] = 2 * x()*z() + 2 * y()*w();
		outData[9] = 2 * y()*z() - 2 * x()*w();
		outData[10] = 1 - 2 * x()*x() - 2 * y()*y();
		outData[11] = 0;
		
		outData[12] = 0;
		outData[13] = 0;
		outData[14] = 0;
		outData[15] = 1;

	}

	/*
	 * Based off the function Quaternion::fromRotationMatrix in the Ogre open source engine.
	 */
	void Quaternion::FromMatrix(const Matrix4x4& matrix)
	{
		Real trace = matrix.A0 + matrix.B1 + matrix.C2;
		Real root;

		const Real * data = matrix.GetDataPtr();

		if (trace > 0.0)
		{
			root = GTEMath::SquareRoot(trace + 1.0f);
			mData[3] = 0.5f*root;
			root = 0.5f / root;
			mData[0] = (matrix.C1 - matrix.B2)*root;
			mData[1] = (matrix.A2 - matrix.C0)*root;
			mData[2] = (matrix.B0 - matrix.A1)*root;
		}
		else
		{
			static UInt32 iNext[3] = { 1, 2, 0 };
			UInt32 i = 0;
			if (matrix.B1 > matrix.A0)
				i = 1;
			if (matrix.C2 > data[i * 4 + i])
				i = 2;
			UInt32 j = iNext[i];
			UInt32 k = iNext[j];

			root = GTEMath::SquareRoot(data[i * 4 + i] - data[j * 4 + j] - data[k * 4 + k] + 1.0f);
			Real * apkQuat[3] = { mData, mData + 1, mData + 2 };
			*apkQuat[i] = 0.5f*root;
			root = 0.5f / root;

			mData[3] = (data[j * 4 + k] - data[k * 4 + j])*root;
			*apkQuat[j] = (data[i * 4 + j] + data[j * 4 + i])*root;
			*apkQuat[k] = (data[i * 4 + k] + data[k * 4 + i])*root;

			//w = (kRot[k][j]-kRot[j][k])*root;
			//*apkQuat[j] = (kRot[j][i]+kRot[i][j])*root;
			//*apkQuat[k] = (kRot[k][i]+kRot[i][k])*root;
		}
	}

	void Quaternion::FromAngleAxis(const Real rfAngle, const Vector3& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		Real fHalfAngle(0.5f * rfAngle);
		Real fSin = GTEMath::Sin(fHalfAngle);
		mData[3] = GTEMath::Cos(fHalfAngle);
		mData[0] = fSin*rkAxis.x;
		mData[1] = fSin*rkAxis.y;
		mData[2] = fSin*rkAxis.z;
	}
	/**
	 * @brief Returns the scaled-axis representation of this
	 * quaternion rotation.
	 */
	/*TVector3 scaledAxis(void) const
	 {
	 Real w[3];
	 HeliMath::scaled_axis_from_quaternion(w, mData);
	 return TVector3(w);
	 }*/

	/**
	 * @brief Sets quaternion to be same as rotation by scaled axis w.
	 */
	/*void scaledAxis(const TVector3& w)
	 {
	 Real theta = w.norm();
	 if (theta > 0.0001)
	 {
	 Real s = sin(theta / 2.0);
	 TVector3 W(w / theta * s);
	 mData[0] = W[0];
	 mData[1] = W[1];
	 mData[2] = W[2];
	 mData[3] = cos(theta / 2.0);
	 } else {
	 mData[0]=mData[1]=mData[2]=0;
	 mData[3]=1.0;
	 }
	 }*/

	/**
	 * @brief Returns a vector rotated by this quaternion.
	 *
	 * Functionally equivalent to:  (rotationMatrix() * v)
	 * or (q * Quaternion(0, v) * q.inverse()).
	 *
	 * @warning conjugate() is used instead of inverse() for better
	 * performance, when this quaternion must be normalized.
	 */

	/*TVector3 rotatedVector(const TVector3& v) const
	 {
	 return (((*this) * Quaternion(v, 0)) * conjugate()).complex();
	 }*/

	/**
	 * @brief Computes the quaternion that is equivalent to a given
	 * euler angle rotation.
	 * @param euler A 3-vector in order:  roll-pitch-yaw.
	 */
	void Quaternion::euler(const Vector3& euler)
	{
		Real c1 = GTEMath::Cos(euler.z * 0.5f * Constants::DegreesToRads);
		Real c2 = GTEMath::Cos(euler.y * 0.5f * Constants::DegreesToRads);
		Real c3 = GTEMath::Cos(euler.x * 0.5f * Constants::DegreesToRads);
		Real s1 = GTEMath::Sin(euler.z * 0.5f * Constants::DegreesToRads);
		Real s2 = GTEMath::Sin(euler.y * 0.5f * Constants::DegreesToRads);
		Real s3 = GTEMath::Sin(euler.x * 0.5f * Constants::DegreesToRads);

		mData[0] = c1 * c2 * s3 - s1 * s2 * c3;
		mData[1] = c1 * s2 * c3 + s1 * c2 * s3;
		mData[2] = s1 * c2 * c3 - c1 * s2 * s3;
		mData[3] = c1 * c2 * c3 + s1 * s2 * s3;
	}

	/** @brief Returns an equivalent euler angle representation of
	 * this quaternion.
	 * @return Euler angles in roll-pitch-yaw order.
	 */
	Vector3 Quaternion::euler(void) const
	{
		Vector3 euler;
		const static Real PI_OVER_2 = Constants::PI * 0.5f;
		const static Real EPSILON = (Real)1e-10;
		Real sqw, sqx, sqy, sqz;

		// quick conversion to Euler angles to give tilt to user
		sqw = mData[3] * mData[3];
		sqx = mData[0] * mData[0];
		sqy = mData[1] * mData[1];
		sqz = mData[2] * mData[2];

		euler.y = asin(2.0f * (mData[3] * mData[1] - mData[0] * mData[2]));
		if (PI_OVER_2 - fabs(euler.y) > EPSILON)
		{
			euler.z = atan2(2.0f * (mData[0] * mData[1] + mData[3] * mData[2]), sqx - sqy - sqz + sqw);
			euler.x = atan2(2.0f * (mData[3] * mData[0] + mData[1] * mData[2]), sqw - sqx - sqy + sqz);
		}
		else
		{
			// compute heading from local 'down' vector
			euler.z = atan2(2 * mData[1] * mData[2] - 2 * mData[0] * mData[3],
				2 * mData[0] * mData[2] + 2 * mData[1] * mData[3]);
			euler.x = 0.0f;

			// If facing down, reverse yaw
			if (euler.y < 0)
				euler.z = Constants::PI - euler.z;
		}
		return euler;
	}

	/**
	 * @brief Computes a special representation that decouples the Z
	 * rotation.
	 *
	 * The decoupled representation is two rotations, Qxy and Qz,
	 * so that Q = Qxy * Qz.
	 */
	/*void decoupleZ(Quaternion* Qxy, Quaternion* Qz) const
	 {
	 TVector3 ztt(0,0,1);
	 TVector3 zbt = this->rotatedVector(ztt);
	 TVector3 axis_xy = ztt.cross(zbt);
	 Real axis_norm = axis_xy.norm();

	 Real axis_theta = acos(HeliMath::saturate(zbt[2], -1,+1));
	 if (axis_norm > 0.00001) {
	 axis_xy = axis_xy * (axis_theta/axis_norm); // limit is *1
	 }

	 Qxy->scaledAxis(axis_xy);
	 *Qz = (Qxy->conjugate() * (*this));
	 }*/

	/**
	 * @brief Returns the quaternion slerped between this and q1 by fraction 0 <= t <= 1.
	 */
	Quaternion Quaternion::slerp(const Quaternion& q1, Real t)
	{
		return slerp(*this, q1, t);
	}

	Quaternion Quaternion::slerp(const Quaternion &qa, const Quaternion &qb, Real t)
	{
		/*  // quaternion to return
		  Quaternion qm;
		  // Calculate angle between them.
		  Real cosHalfTheta = qa.w() * qb.w() + qa.x() * qb.x() + qa.y() * qb.y() + qa.z() * qb.z();
		  // if qa=qb or qa=-qb then theta = 0 and we can return qa
		  if (abs(cosHalfTheta) >= 1.0)
		  {
		  qm.mData[3] = qa.w();
		  qm.mData[0] = qa.x();
		  qm.mData[1] = qa.y();
		  qm.mData[2] = qa.z();
		  return qm;
		  }
		  // Calculate temporary values.
		  Real halfTheta = acos(cosHalfTheta);
		  Real sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
		  // if theta = 180 degrees then result is not fully defined
		  // we could rotate around any axis normal to qa or qb
		  if (fabs(sinHalfTheta) < 0.001)
		  { // fabs is floating point absolute
		  qm.mData[3] = (qa.w() * 0.5 + qb.w() * 0.5);
		  qm.mData[0] = (qa.x() * 0.5 + qb.x() * 0.5);
		  qm.mData[1] = (qa.y() * 0.5 + qb.y() * 0.5);
		  qm.mData[2] = (qa.z() * 0.5 + qb.z() * 0.5);
		  return qm;
		  }
		  Real ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
		  Real ratioB = sin(t * halfTheta) / sinHalfTheta;
		  //calculate Quaternion.
		  qm.mData[3] = (qa.w() * ratioA + qb.w() * ratioB);
		  qm.mData[0] = (qa.x() * ratioA + qb.x() * ratioB);
		  qm.mData[1] = (qa.y() * ratioA + qb.y() * ratioB);
		  qm.mData[2] = (qa.z() * ratioA + qb.z() * ratioB);
		  return qm;*/

		// calc cosine theta
		Real cosom = qa.x() * qb.x() + qa.y() * qb.y() + qa.z() * qb.z() + qa.w() * qb.w();
		// adjust signs (if necessary)
		Quaternion end = qb;
		if (cosom < static_cast<Real>(0.0))
		{
			cosom = -cosom;
			end.mData[0] = -end.x(); // Reverse all signs
			end.mData[1] = -end.y();
			end.mData[2] = -end.z();
			end.mData[3] = -end.w();
		}
		// Calculate coefficients
		Real sclp, sclq;
		if ((static_cast<Real>(1.0) - cosom) > static_cast<Real>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			Real omega, sinom;
			omega = std::acos(cosom); // extract theta from dot product's cos theta
			sinom = std::sin(omega);
			sclp = std::sin((static_cast<Real>(1.0) - t) * omega) / sinom;
			sclq = std::sin(t * omega) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<Real>(1.0) - t;
			sclq = t;
		}

		Quaternion qOut;

		Real x = sclp * qa.x() + sclq * end.x();
		Real y = sclp * qa.y() + sclq * end.y();
		Real z = sclp * qa.z() + sclq * end.z();
		Real w = sclp * qa.w() + sclq * end.w();

		qOut.Set(x, y, z, w);
		return qOut;
	}

	/// Returns quaternion that is slerped by fraction 't' between q0 and q1.
	/*Quaternion Quaternion::slerp(const Quaternion& q0, const Quaternion& q1, Real t)
	 {
	 Real omega = acos(HeliMath::saturate(q0.mData[0]*q1.mData[0] +
	 q0.mData[1]*q1.mData[1] +
	 q0.mData[2]*q1.mData[2] +
	 q0.mData[3]*q1.mData[3], -1,1));
	 if (fabs(omega) < 1e-10) {
	 omega = 1e-10;
	 }
	 Real som = sin(omega);
	 Real st0 = sin((1-t) * omega) / som;
	 Real st1 = sin(t * omega) / som;

	 return Quaternion(q0.mData[0]*st0 + q1.mData[0]*st1,
	 q0.mData[1]*st0 + q1.mData[1]*st1,
	 q0.mData[2]*st0 + q1.mData[2]*st1,
	 q0.mData[3]*st0 + q1.mData[3]*st1);
	 }*/

	/**
	 * @brief Returns pointer to the internal array.
	 *
	 * Array is in order x,y,z,w.
	 */
	Real* Quaternion::row(uint32_t i)
	{
		return mData + i;
	}
	// Const version of the above.
	const Real* Quaternion::row(uint32_t i) const
	{
		return mData + i;
	}



	Quaternion Quaternion::getRotation(const Vector3& source, const Vector3& dest)
	{
		return getRotation(source, dest, Vector3::Zero);
	}

	Quaternion Quaternion::getRotation(const Vector3& source, const Vector3& dest, const Vector3& fallbackAxis)
	{
		// Based on Stan Melax's article in Game Programming Gems
		Quaternion q;

		// Copy, since cannot modify local
		Vector3 v0 = source;
		Vector3 v1 = dest;
		v0.Normalize();
		v1.Normalize();

		Real d = Vector3::Dot(v0, v1);

		// If dot == 1, vectors are the same
		if (d >= 1.0f)
		{
			return Quaternion::Identity;
		}

		Vector3 axis;
		if (d < (1e-6f - 1.0f))
		{
			if (!(fallbackAxis == Vector3::Zero))
			{
				// rotate 180 degrees about the fallback axis
				q.FromAngleAxis(Constants::PI, fallbackAxis);
			}
			else
			{
				// Generate an axis
				Vector3::Cross(Vector3::UnitX, source, axis);

				if (axis.IsZeroLength()) // pick another if colinear
				{
					Vector3::Cross(Vector3::UnitY, source, axis);
				}

				axis.Normalize();
				q.FromAngleAxis(Constants::PI, axis);
			}
		}
		else
		{
			Real s = GTEMath::SquareRoot((1 + d) * 2);
			Real invs = 1 / s;

			Vector3 c;
			Vector3::Cross(v0, v1, c);

			q.Set(c.x * invs, c.y * invs, c.z * invs, s * 0.5f);
			q.normalize();
		}

		return q;
	}

	/**
	 * @brief Global operator allowing left-multiply by scalar.
	 */
	Quaternion operator*(Real s, const Quaternion& q);
}

