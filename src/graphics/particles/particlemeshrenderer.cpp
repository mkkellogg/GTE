#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "object/engineobjectmanager.h"
#include "scene/sceneobjectcomponent.h"
#include "scene/eventmanager.h"
#include "scene/sceneobject.h"
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
