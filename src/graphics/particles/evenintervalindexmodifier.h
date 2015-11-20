
/*
* class: EvenIntervalIndexModifier
*
* author: Mark Kellogg
*
* A particle modifier generates an increasing integer value at an even
* interval based on a specified integer range and the particle's age.
*
*/

#ifndef _GTE_EVENINTERVALINDEX_MODIFIER_H_
#define _GTE_EVENINTERVALINDEX_MODIFIER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "particlemodifier.h"
#include "particle.h"

#include <vector>
#include <string>

namespace GTE
{
	class EvenIntervalIndexModifier : public ParticleModifier<UInt32>
	{
		UInt32 totalSteps;

		public:

		EvenIntervalIndexModifier(UInt32 totalSteps) : ParticleModifier<UInt32>(false)
		{
			this->totalSteps = totalSteps;
		}

		virtual ~EvenIntervalIndexModifier()
		{

		}

		void Initialize(Particle& particle, UInt32& targetAttribute) const override
		{
			targetAttribute = 0;
		}

		void Update(Particle& particle, UInt32& targetAttribute, Real t) const override
		{
			if(!runOnce)
			{
				Real fraction = particle.Age / particle.LifeSpan;
				UInt32 step = (UInt32)(fraction * totalSteps);
				if(step == totalSteps && step > 0) step--;
				targetAttribute = step;
			}
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
