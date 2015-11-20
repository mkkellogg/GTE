#ifndef _GTE_OBJECTPAIRKEY_H_
#define _GTE_OBJECTPAIRKEY_H_

#include "engine.h"
#include "object/engineobject.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace GTE
{
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
			Int32 operator()(const ObjectPairKey& s) const
			{
				return ((Int32)s.ObjectAID << 1) + ((Int32)s.ObjectBID << 2);
			}
		}ObjectPairKeyHasher;

		typedef struct
		{
			Bool operator() (const ObjectPairKey& a, const ObjectPairKey& b) const { return a == b; }
		} ObjectPairKeyEq;

		Bool operator==(const ObjectPairKey& s) const
		{
			return s.ObjectAID == this->ObjectAID && s.ObjectBID == this->ObjectBID;
		}
	};
}

#endif
