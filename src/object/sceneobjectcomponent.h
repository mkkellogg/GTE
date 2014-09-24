#ifndef _SCENEOBJECTCOMPONENT_H_
#define _SCENEOBJECTCOMPONENT_H_

// forward declaration
class SceneObjectComponent;

#include "object/engineobject.h"

class SceneObjectComponent : public EngineObject
{
	friend class EngineObjectManager;

    protected:

	SceneObjectComponent();
    virtual ~SceneObjectComponent();

    public :
};

#endif
