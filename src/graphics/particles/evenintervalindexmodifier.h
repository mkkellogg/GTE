
/*
* class: EvenIntervalIndexModifier
*
* author: Mark Kellogg
*
* A particle modifier that generates an increasing integer value at an even
* interval based on a specified integer range and the particle's age.
*
*/

#ifndef _GTE_EVENINTERVALINDEX_MODIFIER_H_
#define _GTE_EVENINTERVALINDEX_MODIFIER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "particlemodifier.h"
#include "particle.h"

namespace GTE
{
	class EvenIntervalIndexModifier : public ParticleModifier<UInt32>
	{
		UInt32 totalSteps;

		public:

		EvenIntervalIndexModifier(UInt32 totalSteps) : ParticleModifier<UInt32>()
		{
			this->totalSteps = totalSteps;
		}

		virtual ~EvenIntervalIndexModifier()
		{

		}

		void Update(Particle& particle, UInt32& targetAttribute, Real t) const override
		{
			Real fraction = particle.Age / particle.LifeSpan;
			UInt32 step = (UInt32)(fraction * totalSteps);
			if(step == totalSteps && step > 0) step--;
			targetAttribute = step;
		}

		ParticleModifier<UInt32>* Clone() const override
		{
			EvenIntervalIndexModifier* baseClone = new EvenIntervalIndexModifier(totalSteps);
			ASSERT(baseClone != nullptr, "EvenIntervalIndexModifier::Clone -> Could not clone modifier.");
			return static_cast<ParticleModifier<UInt32>*>(baseClone);
		}
	};
}

#endif
