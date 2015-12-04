#ifndef _GTE_VECTOR2_H_
#define _GTE_VECTOR2_H_

#include "vector2.h"
#include "base/basevector2.h"
#include "base/basevector2array.h"

namespace GTE
{
	class Vector2 : public BaseVector2
	{
	protected:

	public:

		Real &x;
		Real &y;

		Vector2();
		Vector2(Bool permAttached, Real * target);
		Vector2(Real u, Real v);
		Vector2(const Vector2& uv);
		~Vector2();

		Vector2 & operator= (const Vector2 & source);
		BaseVector2& operator= (const BaseVector2& source);
		void Set(Real x, Real y);
		Real MaxComponentMagnitude();

		void Add(const Vector2& v);
		static void Add(const Vector2& v1, const Vector2& v2, Vector2& result);
		void Multiply(const Vector2& v);
		static void Multiply(const Vector2& a, const Vector2& b, Vector2& results);
		void Lerp(const Vector2& v1, const Vector2& v2, Real t);
		static void Lerp(const Vector2& v1, const Vector2& v2, Vector2& result, Real t);

		void Scale(Real magnitude);
		void Normalize();
		Real Magnitude() const;
		static Real Magnitude(Real x, Real y);

		virtual void AttachTo(Real * data);
		virtual void Detach();
	};

	typedef BaseVector2Array<Vector2> Vector2Array;
}

#endif
