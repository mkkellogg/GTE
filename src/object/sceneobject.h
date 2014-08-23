#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;

#include "engineobject.h"

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	SceneObject();
    virtual ~SceneObject();
};

#endif
