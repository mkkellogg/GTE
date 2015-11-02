
/*
* class: ParticleUtil
*
* author: Mark Kellogg
*
* General utility functions for particle systems.
*
*/

#ifndef _GTE_PARTICLE_UTIL_H_
#define _GTE_PARTICLE_UTIL_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "gtemath/gtemath.h"
#include "particles.h"
#include <vector>
#include <string>

namespace GTE
{
	// forward delcarations
	class BaseVector4;
	class Vector3;
	class Vector2;
	class Point3;
	class Color4;

	class ParticleUtil
	{
		static void RandomizeVectorObject(Point3& target);
		static void RandomizeVectorObject(Vector3& target);
		static void RandomizeVectorObject(Vector2& target);
		static void RandomizeVectorObject(Color4& target);

		public:
		
		template <class T> static void GetRandom(const T& offset, const T& range, T& target, Bool edgeClamp, ParticleRangeType rangeType)
		{
			RandomizeVectorObject(target);
			if(rangeType == ParticleRangeType::Sphere)
			{
				target.Normalize();
			}

			if(rangeType == ParticleRangeType::Sphere)
			{
				target.Multiply(range);
				if(!edgeClamp)target.Scale(GTEMath::Random() * 2.0f - 1.0f);
			}
			else if(rangeType == ParticleRangeType::Cube)
			{
				if(edgeClamp)target.Scale(1.0f / target.MaxComponentMagnitude());
				target.Multiply(range);
			}

			target.Add(offset);
		}

		template <> static void GetRandom<Real>(const Real& offset, const Real& range, Real& target, Bool edgeClamp, ParticleRangeType rangeType)
		{
			target = offset + range * (GTEMath::Random() - 0.5f);
		}

		template <class T> static void Lerp(const T& a, const T& b, T& target, Real t)
		{
			target.Lerp(a, b, t);
		}

		template <> static void Lerp<Real>(const Real& a, const Real& b, Real& target, Real t)
		{
			target = a + t * (b - a);
		}

	};
}

#endif