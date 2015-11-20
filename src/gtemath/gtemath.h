#ifndef _GTE_GTEMATH_H_
#define _GTE_GTEMATH_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	class GTEMath
	{
	public:

		static Real InverseSquareRoot(Real n);
		static Real QuickInverseSquareRoot(Real n);
		static Real SquareRoot(Real n);
		static Real QuickSquareRoot(Real n);
		static Real Round(Real n);
		static Real Cos(Real n);
		static Real ACos(Real n);
		static Real Sin(Real n);
		static Real Tan(Real n);
		static Real Abs(Real n);
		static Real Min(Real a, Real b);
		static Real Max(Real a, Real b);
		static Int32 Max(Int32 a, Int32 b);
		static Real Lerp(Real a, Real b, Real t);
		static Real Random();
	};
}

#endif
