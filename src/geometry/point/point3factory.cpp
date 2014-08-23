#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "point3factory.h"
#include "point3.h"
#include "base/basevector4.h"

Point3Factory * Point3Factory::instance;

Point3Factory::Point3Factory()
{

}

Point3Factory:: ~Point3Factory()
{

}

BaseVector4 * Point3Factory::CreatePermAttached(float * target)
{
	return new Point3(true, target);
}

void Point3Factory::CreateArray(int count, BaseVector4 *** ppptr)
{
	*ppptr = (BaseVector4**)new Point3*[count];
	if(*ppptr != NULL)
	{
		memset(*ppptr, (int)NULL, sizeof(Point3*) * count);
	}
}

Point3Factory * Point3Factory::GetInstance()
{
	if(instance == NULL)
	{
		instance = new Point3Factory();
	}

	return instance;
}
