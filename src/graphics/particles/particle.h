
/*
* class: Particle
*
* author: Mark Kellogg
*
* Container class for particle data.
*/

#ifndef _GTE_PARTICLE_H_
#define _GTE_PARTICLE_H_

#include "engine.h"
#include "object/engineobject.h"
#include "gtemath/gtemath.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3.h"
#include "graphics/color/color4.h"
#include "particles.h"

namespace GTE {
    class Particle {
        friend class ParticleSystem;

        Vector3 _tempVector3;
        Point3 _tempPoint3;

    public:

        Vector2 Size;
        Color4 Color;
        Real Alpha;
        UInt32 AtlasIndex;
        Bool Alive;
        Real Age;
        Real LifeSpan;

        Point3 Position;
        Vector3 Velocity;
        Vector3 Acceleration;

        Real Rotation = 0.0f;
        Real RotationalSpeed = 0.0f;
        Real RotationalAcceleration = 0.0f;

        Particle() {
            Alpha = 1.0f;
            Age = 0.0f;
            AtlasIndex = 0;
            Alive = false;
            LifeSpan = 0.0f;

            Rotation = 0.0f;
            RotationalSpeed = 0.0f;
            RotationalAcceleration = 0.0f;
        }
    };
}

#endif
