#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobject.h"

EngineObject::EngineObject() : objectID((unsigned long)-1L)
{

}

EngineObject::~EngineObject()
{

}

unsigned long EngineObject::GetObjectID() const
{
	return objectID;
}

void EngineObject::SetObjectID(unsigned long id)
{
	objectID = id;
}

