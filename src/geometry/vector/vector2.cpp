#include "vector2.h"
#include "gtemath/gtemath.h"

namespace GTE
{
	Vector2::Vector2() : BaseVector<Vector2>(), x(data[0]), y(data[1])
	{
	}

	Vector2::Vector2(Bool permAttached, Real * target) : BaseVector<Vector2>(permAttached, target), x(data[0]), y(data[1])
	{
	}

	Vector2::Vector2(Real x, Real y) : BaseVector<Vector2>(), x(data[0]), y(data[1])
	{
		Set(x, y);
	}

	Vector2::Vector2(const Vector2& uv) : BaseVector<Vector2>(uv), x(data[0]), y(data[1])
	{
	}

	Vector2::~Vector2()
	{
	}

	/*
	 * Assignment operator
	 */
	Vector2 & Vector2::operator= (const Vector2 & source)
	{
		if (this == &source)return *this;
		BaseVector<Vector2>::operator=(source);
		return *this;
	}

	/*
	 * Over-ridden assignment operator from BaseVector2
	 */
	BaseVector<Vector2>& Vector2::operator= (const  BaseVector<Vector2>& source)
	{
		if (this == &source)return *this;
		BaseVector<Vector2>::operator=(source);
		return *this;
	}

	/*
	* Set the values for each component of this vector.
	*/
	void Vector2::Set(Real x, Real y)
	{
		data[0] = x;
		data[1] = y;
	}

	/*
	* Get the maximum of the respective absolute value of each component.
	*/
	Real Vector2::MaxComponentMagnitude()
	{
		return GTEMath::Max(GTEMath::Abs(x), GTEMath::Abs(y));
	}

	/*
	* Add vector [v] to this vector
	*/
	void Vector2::Add(const Vector2& vector)
	{
		Add(*this, vector, *this);
	}

	/*
	* Add [v1] to [v2] and store the result in [result]
	*/
	void Vector2::Add(const Vector2& v1, const Vector2& v2, Vector2& result)
	{
		result.x = v1.x + v2.x;
		result.y = v1.y + v2.y;
	}

	/*
	* Component-wise multiply this vector with [v].
	*/
	void Vector2::Multiply(const Vector2& v)
	{
		Multiply(*this, v, *this);
	}

	/*
	* Component-wise multiply [a] with [b], and store the result in [result].
	*/
	void Vector2::Multiply(const Vector2& a, const Vector2& b, Vector2& result)
	{
		result.x = a.x * b.x;
		result.y = a.y * b.y;
	}

	/*
	* Linearly interpolate this vector from [v1] to [v2].
	*/
	void Vector2::Lerp(const Vector2& v1, const Vector2& v2, Real t)
	{
		Lerp(v1, v2, *this, t);
	}


	/*
	* Linearly interpolate from [v1] to [v2] and store the result in [result]
	*/
	void Vector2::Lerp(const Vector2& v1, const Vector2& v2, Vector2& result, Real t)
	{
		result.x = ((v2.x - v1.x) * t) + v1.x;
		result.y = ((v2.y - v1.y) * t) + v1.y;
	}

	/*
	* Scale this vector by [magnitude]
	*/
	void Vector2::Scale(Real magnitude)
	{
		x *= magnitude;
		y *= magnitude;
	}

	/*
	* Convert this vector to a unit-vector
	*/
	void Vector2::Normalize()
	{
		Real magnitude = Magnitude();
		if(magnitude != 0)
		{
			Scale(1 / magnitude);
		}
	}

	/*
	* Get the length of this vector
	*/
	Real Vector2::Magnitude() const
	{
		return Magnitude(x, y);
	}

	/*
	* Get the length of the vector specified by [x], [y]
	*/
	Real Vector2::Magnitude(Real x, Real y)
	{
		return (Real)GTEMath::SquareRoot(x*x + y*y);
	}

	/*
	* Override BaseVector2::AttachTo() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references u & v would point to invalid locations.
	*/
	void Vector2::AttachTo(Real * data)
	{
	}

	/*
	* Override BaseVector2::Detach() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references u & v would point to invalid locations.
	*/
	void Vector2::Detach()
	{
	}
}
