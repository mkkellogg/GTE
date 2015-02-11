#ifndef _GTE_SCENEOBJECTCOMPONENT_H_
#define _GTE_SCENEOBJECTCOMPONENT_H_

// forward declaration
class SceneObjectComponent;
class SceneObject;

#include "object/engineobject.h"
#include "object/enginetypes.h"

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

    public :

    SceneObjectRef GetSceneObject();
};

#endif
