#ifndef _GTE_SCENEOBJECTCOMPONENT_H_
#define _GTE_SCENEOBJECTCOMPONENT_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"

namespace GTE
{
	// forward declarations
	class SceneObjectComponent;
	class SceneObject;

	class SceneObjectComponentProcessingDescriptor
	{
		public:

		Bool Awake;
		Bool Started;

		SceneObjectComponentProcessingDescriptor()
		{
			Awake = false;
			Started = false;
		}
	};

	class SceneObjectComponent : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		// SceneObject needs access because it's the container class
		friend class SceneObject;	
		// Event manager needs to be able to call protected event functions
		friend class EventManager;

	protected:

		SceneObjectComponentProcessingDescriptor processingDescriptor;
		SceneObjectSharedPtr sceneObject;

		SceneObjectComponentProcessingDescriptor& GetProcessingDescriptor();

		SceneObjectComponent();
		virtual ~SceneObjectComponent();

		void SetSceneObject(SceneObjectRef sceneObject);

		virtual void Awake();
		virtual void Start();
		virtual void Update();
		virtual void OnWillRender();

	public:

		SceneObjectRef GetSceneObject();
	
	};
}

#endif
