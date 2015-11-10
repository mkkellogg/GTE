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

		SceneObjectSharedPtr sceneObject;

		SceneObjectComponent();
		virtual ~SceneObjectComponent();

		virtual void Update();
		virtual void OnWillRender(CameraRef camera);

	public:

		SceneObjectRef GetSceneObject();
	
	};
}

#endif
