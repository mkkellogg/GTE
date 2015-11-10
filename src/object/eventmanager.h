#ifndef _GTE_EVENTMANAGER_H_
#define _GTE_EVENTMANAGER_H_

#include <string>
#include <map>
#include "base/intmask.h"
#include "object/enginetypes.h"

namespace GTE
{
	//forward declarations
	class EngineObjectManager;
	class SceneObjectComponent;

	enum class SceneObjectEvent
	{
		Awake = 0,
		Start = 2,
		Update = 3,
		WillRender = 4
	};

	class EventManager
	{
		friend class ForwardRenderManager;
		friend class Engine;

		protected:
				
		EventManager();
		~EventManager();

		Bool Init();

		void DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObject& object);
		void DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObjectComponent& object);

		public:
		
	};
}

#endif
