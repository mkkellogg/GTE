#ifndef _GTE_SCENEOBJECTCOMPONENT_H_
#define _GTE_SCENEOBJECTCOMPONENT_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"

namespace GTE
{
	// forward declaration
	class SceneObjectComponent;
	class SceneObject;

	class SceneObjectComponent : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		friend class SceneObject;

	protected:

		SceneObjectRef sceneObject;

		SceneObjectComponent();
		virtual ~SceneObjectComponent();

	public:

		SceneObjectRef GetSceneObject();
		virtual void Update();
	};
}

#endif
