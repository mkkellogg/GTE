#ifndef _ENGINEOBJECT_H_
#define _ENGINEOBJECT_H_

//forward declarations
class EngineObjectManager;

class EngineObject
{
	friend class EngineObjectManager;

	unsigned long objectID;
	void SetObjectID(unsigned long id);

    protected:

	EngineObject();
	virtual ~EngineObject();

    public:

	unsigned long GetObjectID() const;
};

#endif
