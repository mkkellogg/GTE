#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "objectpairkey.h"

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
