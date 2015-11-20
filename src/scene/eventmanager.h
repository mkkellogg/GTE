#ifndef _GTE_EVENTMANAGER_H_
#define _GTE_EVENTMANAGER_H_

#include "engine.h"
#include "base/intmask.h"

#include <string>
#include <map>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;
	class SceneObject;
	class SceneObjectComponent;

	enum class SceneObjectEvent
	{
		Awake = 0,
		Start = 2,
		Update = 3,
		PreRender = 4,
		WillRender = 5
	};

	class EventManager
	{
		// necessary to dispatch render events
		friend class ForwardRenderManager;
		// Engine is the manager class for EventManager. it needs to be able to instantiate it
		friend class Engine;
		// necessary to dispatch update events
		friend class SceneManager;

		protected:
				
		EventManager();
		~EventManager();

		Bool Init();

		void DispatchSceneObjectComponentEvent(SceneObjectEvent evnt, SceneObjectComponent& component);
		void DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObject& object);

		public:
		
	};
}

#endif
