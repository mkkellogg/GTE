
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

#include "engine.h"
#include "object/engineobject.h"
#include "particlemodifier.h"
#include "particleutil.h"
#include "particles.h"
#include "global/global.h"
#include "global/constants.h"
#include "engine.h"
#include "global/assert.h"

namespace GTE
{
	template <typename T> class RandomModifier : public ParticleModifier<T>
	{
		T offset;
		T range;
		ParticleRangeType rangeType;
		Bool edgeClamp;

		public:

		RandomModifier(const T& offset, const T& range, ParticleRangeType rangeType, Bool edgeClamp) : ParticleModifier<T>()
		{
			this->offset = offset;
			this->range = range;
			this->rangeType = rangeType;
			this->edgeClamp = edgeClamp;
		}

		virtual ~RandomModifier()
		{

		}

		void Update(Particle& particle, T& targetAttribute, Real t) const override
		{
			ParticleUtil::GetRandom(offset, range, targetAttribute, edgeClamp, rangeType);
		}

		ParticleModifier<T>* Clone() const override
		{
			RandomModifier<T> * baseClone = new RandomModifier<T>(offset, range, rangeType, edgeClamp);
			ASSERT(baseClone != nullptr, "RandomModifer<T>::Clone -> Could not clone modifier.");
			return static_cast<ParticleModifier<T>*>(baseClone);
		}
	};
}

#endif
