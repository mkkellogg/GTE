#ifndef _SCENEOBJECTCOMPONENT_H_
#define _SCENEOBJECTCOMPONENT_H_

// forward declaration
class SceneObjectComponent;
class SceneObject;

#include "object/engineobject.h"

class SceneObjectComponent : public EngineObject
{
	friend class EngineObjectManager;
	friend class SceneObject;

    protected:

	SceneObject * sceneObject;

	SceneObjectComponent();
    virtual ~SceneObjectComponent();

    public :
};

#endif
