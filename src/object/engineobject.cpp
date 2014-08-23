#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobject.h"

EngineObject::EngineObject() : objectID(-1)
{

}

EngineObject::~EngineObject()
{

}

int EngineObject::GetObjectID()
{
	return objectID;
}
