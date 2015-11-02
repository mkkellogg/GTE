
/*
* class: FrameSetModifier
*
* author: Mark Kellogg
*
* A particle modifier that contains a set of key frames which each contain
* a value for a specified attribute . At a given point in time, the modifier
* interpolates between the two frames that most closely correspond to that point 
* in time to determine the value for that attribute.
*
*/

#ifndef _GTE_FRAMESET_MODIFIER_H_
#define _GTE_FRAMESET_MODIFIER_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "particlemodifier.h"
#include "particleframeset.h"
#include <vector>
#include <string>

namespace GTE
{
	template <typename T> class FrameSetModifier : public ParticleModifier<T>
	{
		ParticleFrameSet<T> frameSet;

		public:

		FrameSetModifier(Bool runOnce) : ParticleModifier(runOnce)
		{

		}

		virtual ~FrameSetModifier()
		{

		}

		void Initialize(Particle& particle, T& targetAttribute) override
		{
			frameSet.InterpolateFrameValues(0.0f, targetAttribute);
		}

		void Update(Particle& particle, T& targetAttribute, Real t) override
		{
			if(!runOnce)
			{
				frameSet.InterpolateFrameValues(t, targetAttribute);
			}
		}
	};
}

#endif