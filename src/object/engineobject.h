#ifndef _GTE_ENGINEOBJECT_H_
#define _GTE_ENGINEOBJECT_H_

//forward declarations
class EngineObjectManager;

typedef unsigned int ObjectID;

class EngineObject
{
	friend class EngineObjectManager;

	ObjectID objectID;
	void SetObjectID(ObjectID id);

    protected:

	EngineObject();
	virtual ~EngineObject();

    public:

	ObjectID GetObjectID() const;
};

#endif
