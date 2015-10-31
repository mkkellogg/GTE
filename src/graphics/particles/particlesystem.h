/*
* class: ParticleSystem
*
* author: Mark Kellogg
*
* Core container class for a particle system.
*
*/

#ifndef _GTE_PARTICLE_SYSTEM_H_
#define _GTE_PARTICLE_SYSTEM_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include <vector>
#include <string>


// forward Declarations

namespace GTE
{
	class ParticleSystem
	{
		friend class Graphics;

		public:

		class Particle
		{
			Vector3 _tempVector3;

			public:

			Vector3 Size;
			Color4 Color;
			Real Alpha;
			UInt32 AtlasIndex;
			Bool Alive;
			Real Age;
			Real LifeSpan;

			Vector3 Position;
			Vector3 Velocity;
			Vector3 Acceleration;

			Real Rotation = 0.0f;
			Real RotationalSpeed = 0.0f;
			Real RotationalAcceleration = 0.0f;

			Particle()
			{
				Alpha = 1.0f;
				Age = 0.0f;
				AtlasIndex = 1;
				Alive = false;
				LifeSpan = 0.0f;

				Rotation = 0.0f;
				RotationalSpeed = 0.0f;
				RotationalAcceleration = 0.0f;
			}
		};

		private:

	

		protected:


		public:

		ParticleSystem();
		virtual ~ParticleSystem();
		
	};
}

#endif
