#ifndef _GTE_OBJECTPAIRKEY_H_
#define _GTE_OBJECTPAIRKEY_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include <vector>
#include <unordered_map>
#include <memory>
/*
 * This class is used as the key in the renderedObjects hashing structure
 */
class ObjectPairKey
{
	public:

	ObjectID ObjectAID;
	ObjectID ObjectBID;

	ObjectPairKey();
	ObjectPairKey(ObjectID objectAID, ObjectID objectBID);

	 // TODO: optimize this hashing function (implement correctly)
	typedef struct
	{
		 int operator()(const ObjectPairKey& s) const
		 {
			  return ((int)s.ObjectAID << 1) +  ((int)s.ObjectBID << 2);
		 }
	}ObjectPairKeyHasher;

	typedef struct
	{
	  bool operator() (const ObjectPairKey& a, const ObjectPairKey& b) const { return a==b; }
	} ObjectPairKeyEq;

	bool operator==(const ObjectPairKey& s) const
	{
		return s.ObjectAID == this->ObjectAID && s.ObjectBID == this->ObjectBID;
	}
};

#endif
