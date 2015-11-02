#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "vector3.h"
#include "gtemath/gtemath.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"
#include "engine.h"

namespace GTE
{
	const Vector3 Vector3::Zero(0, 0, 0);
	const Vector3 Vector3::UnitX(1, 0, 0);
	const Vector3 Vector3::UnitY(0, 1, 0);
	const Vector3 Vector3::UnitZ(0, 0, 1);
	const Vector3 Vector3::Up(0, 1, 0);

	// TODO: make this properly account for left-handed vs. right handed
	const Vector3 Vector3::Forward(0, 0, -1);

	/*
	 * Default constructor
	 */
	Vector3::Vector3() : BaseVector4(), x(data[0]), y(data[1]), z(data[2])
	{

	}

	/*
	 * Constructor will alternate backing storage
	 */
	Vector3::Vector3(Bool permAttached, Real * target) : BaseVector4(permAttached, target), x(data[0]), y(data[1]), z(data[2])
	{


	}

	/*
	 * Constructor with initialization values
	 */
	Vector3::Vector3(Real x, Real y, Real z) : BaseVector4(x, y, z, 0), x(data[0]), y(data[1]), z(data[2])
	{

	}

	/*
	 * Copy constructor
	 */
	Vector3::Vector3(const Vector3& vector) : BaseVector4(vector), x(data[0]), y(data[1]), z(data[2])
	{

	}

	/*
	 * Assignment operator
	 */
	Vector3& Vector3::operator=(const Vector3& source)
	{
		if (this == &source)return *this;
		BaseVector4::operator=(source);
		return *this;
	}

	/*
	 * Comparison operator
	 */
	Bool Vector3::operator==(const Vector3 & source) const
	{
		Real epsilon = .005f;
		return GTEMath::Abs(source.x - this->x) < epsilon && GTEMath::Abs(source.y - this->y) < epsilon && GTEMath::Abs(source.z - this->z) < epsilon;
	}

	/*
	 * Comparison operator
	 */
	Bool Vector3::operator==(const Vector3 & source)
	{
		Real epsilon = .005f;
		return GTEMath::Abs(source.x - this->x) < epsilon && GTEMath::Abs(source.y - this->y) < epsilon && GTEMath::Abs(source.z - this->z) < epsilon;
	}


	/*
	 * Test for exact equality
	 */
	Bool Vector3::AreStrictlyEqual(const Vector3* a, const Vector3* b)
	{
		NONFATAL_ASSERT_RTRN(a != nullptr && b != nullptr, "Vector3::AreStrictlyEqual -> Null vector passed.", false, true);

		return a->x == b->x && a->y == b->y && a->z == b->z;
	}

	/*
	 * Clean up
	 */
	Vector3::~Vector3()
	{

	}

	/*
	* Set the values of this vector
	*/
	void Vector3::Set(Real x, Real y, Real z)
	{
		BaseVector4::Set(x, y, z, 0);
	}

	/*
	* Get the maximum of the respective absolute value of each component.
	*/
	Real Vector3::MaxComponentMagnitude()
	{
		return GTEMath::Max(GTEMath::Abs(x), GTEMath::Max(GTEMath::Abs(y), GTEMath::Abs(z)));
	}

	/*
	 * Add vector [v] to this vector
	 */
	void Vector3::Add(const Vector3& v)
	{
		Add(*this, v, *this);
	}

	/*
	 * Add [v1] to [v2] and store the result in [result]
	 */
	void Vector3::Add(const Vector3& v1, const Vector3& v2, Vector3& result)
	{
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
		result.z = v1.z + v2.z;
	}

	/*
	 * Subtract [v2] from [v1] and store the result in [result]
	 */
	void Vector3::Subtract(const Vector3& v1, const Vector3& v2, Vector3& result)
	{
		result.x = v1.x - v2.x;
		result.y = v1.y - v2.y;
		result.z = v1.z - v2.z;
	}

	/*
	* Component-wise multiply this vector with [v].
	*/
	void Vector3::Multiply(const Vector3& v)
	{
		Multiply(*this, v, *this);
	}

	/*
	* Component-wise multiply [a] with [b], and store the result in [result].
	*/
	void Vector3::Multiply(const Vector3& a, const Vector3& b, Vector3& result)
	{
		result.x = a.x * b.x;
		result.y = a.y * b.y;
		result.z = a.z * b.z;
	}

	/*
	* Linearly interpolate this vector from [v1] to [v2].
	*/
	void Vector3::Lerp(const Vector3& v1, const Vector3& v2, Real t)
	{
		Lerp(v1, v2, *this, t);
	}

	/*
	* Linearly interpolate from [v1] to [v2] and store the result in [result]
	*/
	void Vector3::Lerp(const Vector3& v1, const Vector3& v2, Vector3& result, Real t)
	{
		result.x = ((v2.x - v1.x) * t) + v1.x;
		result.y = ((v2.y - v1.y) * t) + v1.y;
		result.z = ((v2.z - v1.z) * t) + v1.z;
	}

	/*
	 * Scale this vector by [magnitude]
	 */
	void Vector3::Scale(Real magnitude)
	{
		x *= magnitude;
		y *= magnitude;
		z *= magnitude;
	}

	/*
	 * Convert this vector to a unit-vector
	 */
	void Vector3::Normalize()
	{
		Real magnitude = Magnitude();
		if (magnitude != 0)
		{
			Scale(1 / magnitude);
		}
	}

	/*
	 * Convert this vector to a unity vector, but use the hacky cheat QuickMagnitude(),
	 * which is not as accurate (or correct) as Magnitude()
	 */
	void Vector3::QuickNormalize()
	{
		Real magnitude = QuickMagnitude();
		if (magnitude != 0)
		{
			Scale(1 / magnitude);
		}
	}

	/*
	 * Get the length of this vector
	 */
	Real Vector3::Magnitude() const
	{
		return Magnitude(x, y, z);
	}

	/*
	 * Get the squared length of this vector
	 */
	Real Vector3::SquareMagnitude() const
	{
		return SquareMagnitude(x, y, z);
	}


	/*
	 * Get the length of the vector specified by [x], [y], [z]
	 */
	Real Vector3::Magnitude(Real x, Real y, Real z)
	{
		return (Real)GTEMath::SquareRoot(x*x + y*y + z*z);
	}

	/*
	 * Get the squared length of the vector specified by [x], [y], [z]
	 */
	Real Vector3::SquareMagnitude(Real x, Real y, Real z)
	{
		return x*x + y*y + z*z;
	}


	/*
	 * A cheat to get this vector's magnitude that is much faster than the traditional way.
	 * Instead of using a standard square root, it uses the cheat square root, which is not
	 * correct, but is often "good enough", and is much faster than the correct square root.
	 */
	Real Vector3::QuickMagnitude() const
	{
		return GTEMath::QuickSquareRoot(x*x + y*y + z*z);
	}

	/*
	 * Reverse this vector's direction
	 */
	void Vector3::Invert()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	/*
	 * Calculate the cross product of [a] x [b], and store the result in [result].
	 */
	void Vector3::Cross(const Vector3& a, const Vector3& b, Vector3& result)
	{
		result.x = (a.y*b.z) - (b.y*a.z);
		result.y = (b.x*a.z) - (a.x*b.z);
		result.z = (a.x*b.y) - (b.x*a.y);
	}

	/*
	 * Calculate a unit-vector perpendicular to the plane formed by [a] & [b]. Store
	 * the result in [result].
	 */
	void Vector3::CalcNormal(const Vector3& a, const Vector3& b, Vector3& result)
	{
		Cross(a, b, result);
		result.Normalize();
	}

	/*
	 * Calculate the dot product: [a] dot [b]
	 */
	Real Vector3::Dot(const Vector3& a, const Vector3& b)
	{
		Real x = a.x * b.x;
		Real y = a.y * b.y;
		Real z = a.z * b.z;
		return x + y + z;
	}

	/*
	 * Rotate [a] towards [b] by [theta] degrees.
	 */
	Bool Vector3::RotateTowards(const Vector3& from, const Vector3& to, Real theta, Vector3& result)
	{
		return RotateTowards(from, to, theta, result, Vector3::UnitY);
	}

	/*
	 * Rotate [a] towards [b] by [theta] degrees.
	 */
	Bool Vector3::RotateTowards(const Vector3& from, const Vector3& to, Real theta, Vector3& result, const Vector3& fallbackAxis)
	{
		// convert theta to radians
		theta *= Constants::DegreesToRads;

		Vector3 newVector;
		Vector3 rotationAxis;
		Vector3 fromCopy = from;
		Vector3 toCopy = to;

		fromCopy.Normalize();
		toCopy.Normalize();

		// calculate angle between [from] and [to]
		Real dot = Vector3::Dot(fromCopy, toCopy);

		// clamp to the range -1.0 ... 1.0 to prevent rounding errors in Acos()
		if (dot < -1.0) dot = -1.0;
		else if (dot > 1.0) dot = 1.0;

		Real thetaDiff = GTEMath::ACos(dot);

		// cap theta so we don't rotate past [to]
		if ((theta > thetaDiff && thetaDiff > 0) || (theta < thetaDiff && thetaDiff < 0))
		{
			theta = thetaDiff;
			result = toCopy;
			return true;
		}

		// form rotation axis
		Vector3::Cross(fromCopy, toCopy, rotationAxis);
		Vector3::Cross(rotationAxis, fromCopy, newVector);

		Vector3 fromScaled;
		Vector3 newScaled;

		// Magnitude() < .001 means vectors [from] and [to] are essentially parallel
		if (newVector.Magnitude() < .001)
		{
			// are these vectors really close? if so, just set [result] to [to]
			if (dot > .99999f)
			{
				result = toCopy;
				return true;
			}

			// at this point we have determined the vectors are essentially opposite, and since we can't form
			// a plane in which to do the rotation, the operation fails.
			return false;
		}
		else
		{
			fromScaled = fromCopy;
			fromScaled.Scale(GTEMath::Cos(theta));

			newVector.Normalize();
			newScaled = newVector;
			newScaled.Scale(GTEMath::Sin(theta));

			Vector3::Add(fromScaled, newScaled, result);
			result.Normalize();
			return true;
		}
	}

	// TODO: Implement!!!
	Real Vector3::AngleBetween(const Vector3 * a, const Vector3 * b, const Vector3 * refRightNormal)
	{
		return 0;
	}

	/*
	 *  Returns true if this vector is zero length. In this case we're actually testing whether or
	 *  not the length is greater than a small epsilon.
	 */

	Bool Vector3::IsZeroLength() const
	{
		Real epsilon = (Real)1e-06;
		Real sqlen = (x * x) + (y * y) + (z * z);
		return (sqlen < (epsilon * epsilon));
	}

	/*
	* Override BaseVector4::AttachTo() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references x, y, & z would point to invalid locations.
	*/
	void Vector3::AttachTo(Real * data)
	{

	}

	/*
	* Override BaseVector4::Detach() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references x, y, & z would point to invalid locations.
	*/
	void Vector3::Detach()
	{

	}

	/*
	public static void avgVectors(Vector3 a, Vector3 b, Vector3 avg)
	{
	avg.X = (a.X+b.X)/2f;
	avg.Y = (a.Y+b.Y)/2f;
	avg.Z = (a.Z+b.Z)/2f;
	}
	public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 avg)
	{
	avg.X = (a.X+b.X+c.X)/3f;
	avg.Y = (a.Y+b.Y+c.Y)/3f;
	avg.Z = (a.Z+b.Z+c.Z)/3f;
	}*/

	/*
	public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 avg)
	{
	avg.X = (a.X+b.X+c.X+d.X)/4f;
	avg.Y = (a.Y+b.Y+c.Y+d.Y)/4f;
	avg.Z = (a.Z+b.Z+c.Z+d.Z)/4f;
	}
	public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 d, Real[] weights, Vector3 avg)
	{
	Real totalWeight =0f;
	for(Int32 i=0; i <4; i++)totalWeight += weights[i];
	avg.X = ((weights[0]*a.X)+(weights[1]*b.X)+(weights[2]*c.X)+(weights[3]*d.X))/totalWeight;
	avg.Y = ((weights[0]*a.Y)+(weights[1]*b.Y)+(weights[2]*c.Y)+(weights[3]*d.Y))/totalWeight;
	avg.Z = ((weights[0]*a.Z)+(weights[1]*b.Z)+(weights[2]*c.Z)+(weights[3]*d.Z))/totalWeight;
	}*/

	/*
	public static Vector3 createNormal(Real x1, Real y1, Real z1, Real x2, Real y2, Real z2, Real x3, Real y3, Real z3)
	{
	Vector3 A = new Vector3(x1,y1,z1, x2,y2,z2);
	Vector3 B = new Vector3(x1,y1,z1, x3,y3,z3);
	Vector3 vec= cross(A,B);
	vec.normalize();
	return vec;
	}*/
}



