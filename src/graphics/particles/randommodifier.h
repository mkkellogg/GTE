
/*
* class: RandomModifier
*
* author: Mark Kellogg
*
* A particle modifier that randomizes a particle's attributes based 
* on configurable parameters.
*
*/

#ifndef _GTE_RANDOM_MODIFIER_H_
#define _GTE_RANDOM_MODIFIER_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "particlemodifier.h"
#include "particleutil.h"
#include "particles.h"
#include "global/global.h"
#include "global/constants.h"
#include "engine.h"
#include "global/assert.h"
#include <vector>
#include <string>

namespace GTE
{
	template <typename T> class RandomModifier : public ParticleModifier<T>
	{
		T offset;
		T range;
		ParticleRangeType rangeType;
		Bool edgeClamp;

		public:

		RandomModifier(const T& offset, const T& range, ParticleRangeType rangeType, Bool edgeClamp, Bool runOnce) : ParticleModifier<T>(runOnce)
		{
			this->offset = offset;
			this->range = range;
			this->rangeType = rangeType;
			this->edgeClamp = edgeClamp;
		}

		virtual ~RandomModifier()
		{

		}

		void Initialize(Particle& particle, T& targetAttribute) override
		{
			ParticleUtil::GetRandom(offset, range, targetAttribute, edgeClamp, rangeType);
		}

		void Update(Particle& particle, T& targetAttribute, Real t) override
		{
			if(!this->runOnce)
			{
				ParticleUtil::GetRandom(offset, range, targetAttribute, edgeClamp, rangeType);
			}
		}

		ParticleModifier<T>* Clone() const override
		{
			RandomModifier<T> * baseClone = new RandomModifier<T>(offset, range, rangeType, edgeClamp, ParticleModifier<T>::runOnce);
			ASSERT(baseClone != nullptr, "RandomModifer<T>::Clone -> Could not clone modifier.");
			return static_cast<ParticleModifier<T>*>(baseClone);
		}
	};
}

#endif
