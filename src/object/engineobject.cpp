#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobject.h"

EngineObject::EngineObject() : objectID(-1)
{
	isActive = true;
}

EngineObject::~EngineObject()
{

}

int EngineObject::GetObjectID()
{
	return objectID;
}

bool EngineObject::IsActive()
{
	return isActive;
}

void EngineObject::SetActive(bool active)
{
	this->isActive = active;
}
