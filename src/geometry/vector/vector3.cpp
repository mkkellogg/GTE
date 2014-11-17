#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "vector3.h"
#include "gtemath/gtemath.h"
#include "ui/debug.h"
#include "global/global.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"

const Vector3 Vector3::Zero(0,0,0);
const Vector3 Vector3::UnitX(1,0,0);
const Vector3 Vector3::UnitY(0,1,0);
const Vector3 Vector3::UnitZ(0,0,1);

/*
 * Default constructor
 */
Vector3::Vector3() : BaseVector4(), x(data[0]), y(data[1]), z(data[2])
{
   
}

/*
 * Constructor will alternate backing storage
 */
Vector3::Vector3(bool permAttached, float * target) : BaseVector4(permAttached, target),  x(data[0]), y(data[1]), z(data[2])
{


}

/*
 * Constructor with initialization values
 */
Vector3::Vector3(float x, float y, float z) : BaseVector4(x,y,z,0), x(data[0]), y(data[1]), z(data[2])
{
    
}

/*
 * Copy constructor
 */
Vector3::Vector3(const Vector3& vector) : BaseVector4(&vector), x(data[0]), y(data[1]), z(data[2])
{

}

/*
 * Copy constructor
 */
Vector3::Vector3(const Vector3 * vector) : BaseVector4(vector),  x(data[0]), y(data[1]), z(data[2])
{
    
}

/*
 * Assignment operator
 */
Vector3& Vector3::operator=(const Vector3& source)
{
    if(this == &source)return *this;
    memcpy((void*)data, (void*)source.data, sizeof(float) * 4);
    memcpy((void*)baseData, (void*)source.baseData, sizeof(float) * 4);
    return *this;
}

/*
 * Comparison operator
 */
bool Vector3::operator==(const Vector3 & source) const
{
	return source.x == x && source.y == y && source.z == z;
}

/*
 * Set the values of this vector
 */
void Vector3::Set(float x, float y, float z)
{
	BaseVector4::Set(x,y,z,0);
}

/*
 * Clean up
 */
Vector3::~Vector3()
{
   
}

/*
 * Add vector [v] to this vector
 */
void Vector3::Add(const Vector3 * vector)
{
	ASSERT_RTRN(vector != NULL, "Vector3::Add -> NULL vector passed.");

    x += vector->x;
    y += vector->y;
    z += vector->z;
}

/*
 * Add [v1] to [v2] and store the result in [result]
 */
void Vector3::Add(const Vector3 * v1,const Vector3 * v2, Vector3 * result)
{
	ASSERT_RTRN(v1 != NULL, "Vector3::Add(Vector3 *, Vector3 *, Vector3 *) -> NULL v1 passed.");
	ASSERT_RTRN(v2 != NULL, "Vector3::Add(Vector3 *, Vector3 *, Vector3 *) -> NULL v2 passed.");
	ASSERT_RTRN(result != NULL, "Vector3::Add(Vector3 *, Vector3 *, Vector3 *) -> NULL result passed.");

    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;
}

/*
 * Subtract [v2] from [v1] and store the result in [result]
 */
void Vector3::Subtract(const Vector3 * v1,const Vector3 * v2, Vector3 * result)
{
	ASSERT_RTRN(v1 != NULL, "Vector3::Subtract(Vector3 *, Vector3 *, Vector3 *) -> NULL v1 passed.");
	ASSERT_RTRN(v2 != NULL, "Vector3::Subtract(Vector3 *, Vector3 *, Vector3 *) -> NULL v2 passed.");
	ASSERT_RTRN(result != NULL, "Vector3::Subtract(Vector3 *, Vector3 *, Vector3 *) -> NULL result passed.");

    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;
}

/*
 * Scale this vector by [magnitude]
 */
void Vector3::Scale(float magnitude)
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
    float magnitude = Magnitude();
    if(magnitude != 0)
    {
        Scale(1/magnitude);
    }
}

/*
 * Convert this vector to a unity vector, but use the hacky cheat QuickMagnitude(),
 * which is not ass accurate (or correct) as Magnitude()
 */
void Vector3::QuickNormlize()
{
    float magnitude = QuickMagnitude();
    if(magnitude != 0)
    {
        Scale(1/magnitude);
    }
}

/*
 * Get the length of this vector
 */
float Vector3::Magnitude() const
{
    return Magnitude(x,y,z);
}

/*
 * Get the squared length of this vector
 */
float Vector3::SquareMagnitude() const
{
    return SquareMagnitude(x,y,z);
}


/*
 * Get the length of the vector specified by [x], [y], [z]
 */
float Vector3::Magnitude(float x, float y, float z)
{
    return (float)GTEMath::SquareRoot(x*x + y*y + z*z);
}

/*
 * Get the squared length of the vector specified by [x], [y], [z]
 */
float Vector3::SquareMagnitude(float x, float y, float z)
{
    return x*x + y*y + z*z;
}


/*
 * A cheat to get this vector's magnitude that is much faster than the traditional way.
 * Instead of using a standard square root, it uses the cheat square root, which is not
 * correct, but is often "good enough", and is much faster than the correct square root.
 */
float Vector3::QuickMagnitude() const
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
void Vector3::Cross(const Vector3 * a,const Vector3 * b, Vector3 * result)
{
	ASSERT_RTRN(a != NULL, "Vector3::Cross(Vector3 *, Vector3 *, Vector3 *) -> NULL a passed.");
	ASSERT_RTRN(b != NULL, "Vector3::Cross(Vector3 *, Vector3 *, Vector3 *) -> NULL b passed.");
	ASSERT_RTRN(result != NULL, "Vector3::Cross(Vector3 *, Vector3 *, Vector3 *) -> NULL result passed.");

    float x,y,z;
    x = (a->y*b->z) - (b->y*a->z);
    y = (b->x*a->z) - (a->x*b->z);
    z = (a->x*b->y) - (b->x*a->y);	
    result->Set(x,y,z);
}

/*
 * Calculate a unit-vector perpendicular to the plane formed by [a] & [b]. Store
 * the result in [result].
 */
void Vector3::CalcNormal(const Vector3 * a,const Vector3 * b, Vector3 * result)
{
	ASSERT_RTRN(a != NULL, "Vector3::CalcNormal(Vector3 *, Vector3 *, Vector3 *) -> NULL a passed.");
	ASSERT_RTRN(b != NULL, "Vector3::CalcNormal(Vector3 *, Vector3 *, Vector3 *) -> NULL b passed.");
	ASSERT_RTRN(result != NULL, "Vector3::CalcNormal(Vector3 *, Vector3 *, Vector3 *) -> NULL result passed.");

    Cross(a,b,result);
    result->Normalize();
}

/*
 * Update the element accessor variables to point to the current backing storage. This is
 * usually called when the vector is attached to new backing storage.
 */
void Vector3::UpdateComponentPointers()
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
 * Calculate the dot product: [a] dot [b]
 */
float Vector3::Dot(const Vector3 * a,const Vector3 * b)
{
	ASSERT(a != NULL, "Vector3::CalcNormal(Vector3 *, Vector3 *, Vector3 *) -> NULL a passed.", 0);
	ASSERT(b != NULL, "Vector3::CalcNormal(Vector3 *, Vector3 *, Vector3 *) -> NULL b passed.", 0);

    float x = a->x * b->x;
    float y = a->y * b->y;
    float z = a->z * b->z;
    return x+y+z;
}

/*
 * Attach to new backing storage in [data]
 */
void Vector3::AttachTo(float * data)
{
	ASSERT_RTRN(data != NULL, "Vector3::AttachTo -> NULL data passed.");

	BaseVector4::AttachTo(data);
	UpdateComponentPointers();
}

/*
 * Detach from existing backing storage
 */
void Vector3::Detach()
{
	BaseVector4::Detach();
	UpdateComponentPointers();
}

/*
 * Rotate [a] towards [b] by [theta] degrees.
 */
void Vector3::RotateTowards(const Vector3 * from, const Vector3 * to,  float theta, Vector3 * result)
{
	ASSERT_RTRN(from != NULL, "Vector3::RotateTowards -> NULL from passed.");
	ASSERT_RTRN(to != NULL, "Vector3::RotateTowards -> NULL to passed.");
	ASSERT_RTRN(result != NULL, "Vector3::RotateTowards -> NULL result passed.");

	theta *= Constants::DegreesToRads;

	Vector3 newVector;
	Vector3 rotationAxis;
	Vector3 fromCopy = *from;
	Vector3 toCopy = *to;
	fromCopy.Normalize();
	toCopy.Normalize();

	Vector3::Cross(&fromCopy, &toCopy, &rotationAxis);
	Vector3::Cross(&rotationAxis, &fromCopy, &newVector);

	Vector3 fromScaled;
	Vector3 newScaled;

	// vectors are parallel
	if(newVector.Magnitude() < .001)
	{
		float xdiff = GTEMath::Abs(fromCopy.x - toCopy.x);
		float ydiff = GTEMath::Abs(fromCopy.y - toCopy.y);
		float zdiff = GTEMath::Abs(fromCopy.z - toCopy.z);

		// vectors [a] and [b] are close enough to not bother with rotation
		if(GTEMath::Abs(xdiff) < .001 && GTEMath::Abs(ydiff) < .001  && GTEMath::Abs(zdiff) < .001)
		{
			*result = fromCopy;
			return;
		}
		else // vectors [a] and [b] are opposite
		{
			float realTheta = theta/Constants::PI;
			float intRealTheta = (int)realTheta;

			realTheta -= (float)intRealTheta;
			float diffAngle = Constants::PI - realTheta;

			Vector3 toOrthoLeft;
			toOrthoLeft.Set(-toCopy.z, 0, toCopy.x);
			toOrthoLeft.Scale(GTEMath::Sin(diffAngle));

			Vector3 toScaled = toCopy;
			toScaled.Scale(GTEMath::Cos(diffAngle));

			Vector3::Add(&toOrthoLeft, &toScaled, result);
			result->Normalize();
			return;
		}
	}
	else
	{
		fromScaled = fromCopy;
		fromScaled.Scale(GTEMath::Cos(theta));

		newVector.Normalize();
		newScaled = newVector;
		newScaled.Scale(GTEMath::Sin(theta));

		Vector3::Add(&fromScaled, &newScaled, result);
		result->Normalize();
		return;
	}
}

// TODO: Implement!!!
float Vector3::AngleBetween(const Vector3 * a,const Vector3 * b,const Vector3 * refRightNormal)
{

    return 0 ;
}

/*
 *  Returns true if this vector is zero length.
 */

bool Vector3::IsZeroLength() const
{
	float sqlen = (x * x) + (y * y) + (z * z);
	return (sqlen < (1e-06 * 1e-06));
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
public static void avgVectors(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float[] weights, Vector3 avg)
{
float totalWeight =0f;
for(int i=0; i <4; i++)totalWeight += weights[i];
avg.X = ((weights[0]*a.X)+(weights[1]*b.X)+(weights[2]*c.X)+(weights[3]*d.X))/totalWeight;
avg.Y = ((weights[0]*a.Y)+(weights[1]*b.Y)+(weights[2]*c.Y)+(weights[3]*d.Y))/totalWeight;
avg.Z = ((weights[0]*a.Z)+(weights[1]*b.Z)+(weights[2]*c.Z)+(weights[3]*d.Z))/totalWeight;
}*/

/*
public static Vector3 createNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
Vector3 A = new Vector3(x1,y1,z1, x2,y2,z2);
Vector3 B = new Vector3(x1,y1,z1, x3,y3,z3);
Vector3 vec= cross(A,B);
vec.normalize();
return vec;
}*/




