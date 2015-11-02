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
#include "particlemodifier.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector2.h"
#include "geometry/matrix4x4.h"
#include "geometry/quaternion.h"
#include "graphics/color/color4.h"
#include <vector>
#include <string>


// forward Declarations

namespace GTE
{
	class ParticleSystem
	{
		friend class Graphics;
		
		private:
	
		Bool zSort;
		Bool simulateInLocalSpace;

		Bool releaseAtOnce;
		UInt32 releaseAtOnceCount = 0;
		Bool hasInitialReleaseOccurred;
		Bool isActive;

		ParticleModifier<UInt32>* atlasModifier;
		ParticleModifier<Color4>* colorModifier;
		ParticleModifier<Real>* alphModifier;
		ParticleModifier<Vector2>* sizeModifier;

		// Particle position and position modifiers (velocity and acceleration)
		ParticleModifier<Vector3>* positionModifier;
		ParticleModifier<Vector3>* velocityModifier;
		ParticleModifier<Vector3>* accelerationModifier;

		// Particle rotation and rotation modifiers (rotational speed and rotational acceleration)
		ParticleModifier<Real>* rotationModifier;
		ParticleModifier<Real>* rotationalSpeedModifier;
		ParticleModifier<Real>* rotationalAccelerationModifier;

		UInt32 particleReleaseRate;
		Real particleLifeSpan;
		Real averageParticleLifeSpan;

		UInt32 liveParticleCount;
		UInt32 deadParticleCount;
		Particle* liveParticleArray;
		Particle* deadParticleArray;

		Particle* _tempParticleArray;

		Real timeSinceLastEmit;
		Bool emitting;
		Real age;
		Real lifespan;

		// temporary storage 
		Vector3 _tempVector3;
		Quaternion _tempQuaternion;
		Matrix4x4 _tempMatrix4;

		protected:

		void CalculateAverageParticleLifeSpan();
		void CalculateMaxParticleCount();

		public:

		ParticleSystem();
		virtual ~ParticleSystem();
		void Destroy();
		
	};
}

#endif
