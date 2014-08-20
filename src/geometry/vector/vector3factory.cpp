#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vector3factory.h"
#include "vector3.h"
#include "base/basevector4.h"

Vector3Factory * Vector3Factory::instance;

Vector3Factory::Vector3Factory()
{

}

Vector3Factory::~Vector3Factory()
{

}

Vector3 * Vector3Factory::CreatePermAttached(float * target)
{
	return new Vector3(true, target);
}

void Vector3Factory::CreateArray(int count, BaseVector4 *** ppptr)
{
	*ppptr = (BaseVector4**)new Vector3*[count];
	if(*ppptr != NULL)
	{
		memset(*ppptr, (int)NULL, sizeof(Vector3*) * count);
	}
}

Vector3Factory * Vector3Factory::GetInstance()
{
	if(instance == NULL)
	{
		instance = new Vector3Factory();
	}

	return instance;
}
