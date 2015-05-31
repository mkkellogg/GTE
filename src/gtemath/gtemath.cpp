#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <cmath>
 
#include "gtemath.h"

namespace GTE
{
	union IntFloatUnion
	{
		Int32 i;
		Real f;
	};

	Real GTEMath::SquareRoot(Real n)
	{
		return sqrt(n);
	}

	Real GTEMath::QuickSquareRoot(Real n)
	{
		IntFloatUnion ifu;

		Real threeHalfs = 1.5f;
		Real x2 = n * .5f;

		ifu.f = n;
		ifu.i = 0x5f375a86 - (ifu.i >> 1);
		ifu.f = ifu.f * (threeHalfs - (x2 * ifu.f * ifu.f));

		return ifu.f * n;
	}

	Real GTEMath::Round(Real n)
	{
		return (Real)floor(n + 0.5f);
	}

	Real GTEMath::InverseSquareRoot(Real n)
	{
		Real root = SquareRoot(n);
		if (root == 0)return 0;
		return 1 / root;
	}

	Real GTEMath::QuickInverseSquareRoot(Real n)
	{
		Real root = QuickSquareRoot(n);
		if (root == 0)return 0;
		return 1 / root;
	}

	Real GTEMath::Cos(Real n)
	{
		return cos(n);
	}

	Real GTEMath::ACos(Real n)
	{
		return acos(n);
	}

	Real GTEMath::Sin(Real n)
	{
		return sin(n);
	}

	Real GTEMath::Abs(Real n)
	{
		return fabs(n);
	}

	Real GTEMath::Min(Real a, Real b)
	{
		if (a < b)return a;
		else return b;
	}

	Real GTEMath::Max(Real a, Real b)
	{
		if (a > b)return a;
		else return b;
	}

	Real GTEMath::Lerp(Real a, Real b, Real t)
	{
		return (b - a) * t + a;
	}
}
