#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobject.h"

EngineObject::EngineObject() : objectID((ObjectID)-1)
{

}

EngineObject::~EngineObject()
{

}

ObjectID EngineObject::GetObjectID() const
{
	return objectID;
}

void EngineObject::SetObjectID(ObjectID id)
{
	objectID = id;
}

