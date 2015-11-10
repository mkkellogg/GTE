#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "eventmanager.h"
#include "sceneobject.h"
#include "sceneobjectcomponent.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/particles/particlesystem.h"

namespace GTE
{
	EventManager::EventManager()
	{

	}

	EventManager::~EventManager()
	{

	}

	Bool EventManager::Init()
	{
		return true;
	}

	void EventManager::DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObject& object)
	{
		CameraRef camera = object.GetCamera();
		if(camera.IsValid())
		{
			DispatchSceneObjectEvent(evnt, camera.GetRef());
		}

		LightRef light = object.GetLight();
		if(light.IsValid())
		{
			DispatchSceneObjectEvent(evnt, light.GetRef());
		}

		Mesh3DRef mesh = object.GetMesh3D();
		if(mesh.IsValid())
		{
			DispatchSceneObjectEvent(evnt, mesh.GetRef());
		}

		ParticleSystemRef particleSystem = object.GetParticleSystem();
		if(particleSystem.IsValid())
		{
			DispatchSceneObjectEvent(evnt, particleSystem.GetRef());
		}
	}

	void EventManager::DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObjectComponent& object)
	{
		switch(evnt)
		{
			case SceneObjectEvent::Awake:
				object.Awake();
			break;
			case SceneObjectEvent::Start:
				object.Start();
			break;
			case SceneObjectEvent::Update:
				object.Update();
			break;
			case SceneObjectEvent::WillRender:
				object.OnWillRender();
			break;
		}
	}
}
