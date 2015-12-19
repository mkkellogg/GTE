
/*
* class: CustomModifier
*
* author: Mark Kellogg
*
* A particle modifier that calls a user defined function to modify
* a particle's attributes.
*
*/

#ifndef _GTE_CUSTOM_MODIFIER_H_
#define _GTE_CUSTOM_MODIFIER_H_

#include "engine.h"
#include "object/engineobject.h"
#include "particlemodifier.h"
#include "global/assert.h"

#include <functional>

namespace GTE
{
	// forward declarations
	class Particle;

	template <typename T> class CustomModifier : public ParticleModifier<T>
	{
		std::function<void(Particle& particle, T& targetAttribute, Real t)> callback;

		public:

		CustomModifier(std::function<void(Particle& particle, T& targetAttribute, Real t)> callback) : ParticleModifier<T>()
		{
			this->callback = callback;
		}

		virtual ~CustomModifier()
		{

		}

		void Update(Particle& particle, T& targetAttribute, Real t) const override
		{
			callback(particle, targetAttribute, t);
		}

		ParticleModifier<T>* Clone() const override
		{
			CustomModifier* baseClone = new CustomModifier(callback);
			ASSERT(baseClone != nullptr, "CustomModifier::Clone -> Could not clone modifier.");
			return static_cast<ParticleModifier<T>*>(baseClone);
		}
	};
}

#endif
