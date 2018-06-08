
/*
* class: ParticleModifier
*
* author: Mark Kellogg
*
* Base class for classes that modify a particle's attributes in a particle system.
*
*/

#ifndef _GTE_PARTICLE_MODIFIER_H_
#define _GTE_PARTICLE_MODIFIER_H_

#include "engine.h"
#include "object/engineobject.h"

namespace GTE {
    //forward declarations
    class Particle;

    template <typename T> class ParticleModifier {
    protected:

    public:

        ParticleModifier() {

        }

        virtual ~ParticleModifier() {

        }

        virtual void Update(Particle& particle, T& targetAttribute, Real t) const = 0;
        virtual ParticleModifier<T>* Clone() const = 0;
    };
}

#endif
