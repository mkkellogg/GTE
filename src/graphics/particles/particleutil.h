
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

#include "engine.h"
#include "object/engineobject.h"
#include "gtemath/gtemath.h"
#include "particles.h"

namespace GTE {
    // forward declarations
    class BaseVector4;
    class Vector3;
    class Vector2;
    class Point3;
    class Color4;

    class ParticleUtil {
        static void RandomizeVectorObject(Point3& target);
        static void RandomizeVectorObject(Vector3& target);
        static void RandomizeVectorObject(Vector2& target);
        static void RandomizeVectorObject(Color4& target);

    public:

        template <typename T> static void GetRandom(const T& offset, const T& range, T& target, Bool edgeClamp, ParticleRangeType rangeType) {
            RandomizeVectorObject(target);
            if (rangeType == ParticleRangeType::Sphere) {
                target.Normalize();
            }

            if (rangeType == ParticleRangeType::Sphere) {
                target.Multiply(range);
                if (!edgeClamp)target.Scale(GTEMath::Random() * 2.0f - 1.0f);
            }
            else if (rangeType == ParticleRangeType::Cube) {
                if (edgeClamp)target.Scale(1.0f / target.MaxComponentMagnitude());
                target.Multiply(range);
            }

            target.Add(offset);
        }

        template <class T> static void Lerp(const T& a, const T& b, T& target, Real t) {
            target.Lerp(a, b, t);
        }
    };

    template <> void ParticleUtil::GetRandom<Real>(const Real& offset, const Real& range, Real& target, Bool edgeClamp, ParticleRangeType rangeType);
    template <> void ParticleUtil::Lerp<Real>(const Real& a, const Real& b, Real& target, Real t);
}

#endif
