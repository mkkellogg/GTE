
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

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include <vector>
#include <string>

namespace GTE
{
	//forward declarations
	class Particle;

	template <typename T> class ParticleModifier
	{
		protected:

		Bool runOnce;

		public:

		ParticleModifier(Bool runOnce)
		{
			this->runOnce = runOnce;
		}

		virtual ~ParticleModifier()
		{

		}

		Bool SetOnce()
		{
			return runOnce;
		}

		Bool RunOnce()
		{
			return runOnce;
		}

		virtual void Initialize(Particle& particle, T& targetAttribute) = 0;
		virtual void Update(Particle& particle, T& targetAttribute, Real t) = 0;
		virtual ParticleModifier<T>* Clone() const = 0;
	};
}

#endif
