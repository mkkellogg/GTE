#ifndef _ENGINEOBJECT_H_
#define _ENGINEOBJECT_H_

//forward declarations
class EngineObjectManager;

class EngineObject
{
	friend class EngineObjectManager;

	int objectID;

    protected:

	EngineObject();
    virtual ~EngineObject();

    public:

    virtual int GetObjectID();
};

#endif
