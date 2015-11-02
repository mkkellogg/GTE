#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "particlesystem.h"
#include "particleutil.h"
#include "particle.h"
#include "object/enginetypes.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "framesetmodifier.h"
#include "randommodifier.h"
#include "evenintervalindexmodifier.h"
#include "engine.h"

namespace GTE
{
	ParticleSystem::ParticleSystem()
	{
		zSort = false;
		simulateInLocalSpace = true;
	
		releaseAtOnce = false;
		releaseAtOnceCount = 0;
		hasInitialReleaseOccurred = false;
		isActive = false;

		atlasModifier = nullptr;
		colorModifier = nullptr;
		alphModifier = nullptr;
		sizeModifier = nullptr;

		// Particle position and position modifiers (velocity and acceleration)
		positionModifier = nullptr;
		velocityModifier = nullptr;
		accelerationModifier = nullptr;

		// Particle rotation and rotation modifiers (rotational speed and rotational acceleration)
		rotationModifier = nullptr;
		rotationalSpeedModifier = nullptr;
		rotationalAccelerationModifier = nullptr;

		particleReleaseRate = 100;
		particleLifeSpan = 1.0f;
		averageParticleLifeSpan = 1.0f;
		CalculateAverageParticleLifeSpan();

		CalculateMaxParticleCount();
		liveParticleCount = 0;
		deadParticleCount = 0;
		liveParticleArray = nullptr;
		deadParticleArray = nullptr;
		_tempParticleArray = nullptr;

		timeSinceLastEmit = 0.0f;
		emitting = true;
		age = 0.0f;
		lifespan = 0.0f;
	}

	ParticleSystem::~ParticleSystem()
	{
		Destroy();
	}

	void ParticleSystem::Destroy()
	{

	}

	void ParticleSystem::CalculateAverageParticleLifeSpan()
	{

	}

	void ParticleSystem::CalculateMaxParticleCount()
	{

	}
}
