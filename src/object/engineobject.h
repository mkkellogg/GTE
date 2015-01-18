#ifndef _GTE_ENGINEOBJECT_H_
#define _GTE_ENGINEOBJECT_H_

//forward declarations
class EngineObjectManager;

typedef unsigned int ObjectID;

class EngineObject
{
	// all instances of EngineObject and classes that derive from
	// EngineObject should have their life-cycle managed by EngineObjectManager
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
