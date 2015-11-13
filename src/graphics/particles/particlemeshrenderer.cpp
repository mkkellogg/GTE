#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/eventmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "particlemeshrenderer.h"
#include "particlesystem.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
	* Default constructor
	*/
	ParticleMeshRenderer::ParticleMeshRenderer()
	{
		
	}

	/*
	 * Clean-up
	 */
	ParticleMeshRenderer::~ParticleMeshRenderer()
	{

	}

	void ParticleMeshRenderer::SetTargetSystemObject(SceneObjectRef particleSystemObject)
	{
		this->particleSystemObject = particleSystemObject;
	}

	/*
	* Handle 'WillRender' lifecycle event
	*/
	void ParticleMeshRenderer::WillRender()
	{
		if(particleSystemObject.IsValid())
		{
			ParticleSystemRef system = particleSystemObject->GetParticleSystem();
			system->WillRender();
		}
	}
}
