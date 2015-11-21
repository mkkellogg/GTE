#include "objectpairkey.h"

namespace GTE
{
	ObjectPairKey::ObjectPairKey()
	{
		ObjectAID = 0;
		ObjectBID = 0;
	}

	ObjectPairKey::ObjectPairKey(ObjectID objectAID, ObjectID objectBID)
	{
		ObjectAID = objectAID;
		ObjectBID = objectBID;
	}
}
