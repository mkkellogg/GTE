#include "particleutil.h"
#include "particles.h"
#include "gtemath/gtemath.h"
#include "base/basevector.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector2.h"
#include "geometry/point/point3.h"
#include "graphics/color/color4.h"
#include "global/global.h"
#include "global/assert.h"
#include "util/engineutility.h"
#include "debug/gtedebug.h"

namespace GTE {
    void ParticleUtil::RandomizeVectorObject(Point3& target) {
        target.Set(GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f);
    }

    void ParticleUtil::RandomizeVectorObject(Vector3& target) {
        target.Set(GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f);
    }

    void ParticleUtil::RandomizeVectorObject(Vector2& target) {
        target.Set(GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f);
    }

    void ParticleUtil::RandomizeVectorObject(Color4& target) {
        target.Set(GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f, GTEMath::Random() - 0.5f);
    }

    template <> void ParticleUtil::GetRandom<Real>(const Real& offset, const Real& range, Real& target, Bool edgeClamp, ParticleRangeType rangeType) {
        target = offset + range * (GTEMath::Random() - 0.5f);
    }

    template <> void ParticleUtil::Lerp<Real>(const Real& a, const Real& b, Real& target, Real t) {
        target = a + t * (b - a);
    }
}
