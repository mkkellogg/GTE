#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uv2factory.h"
#include "uv2.h"
#include "base/basevector2.h"

UV2Factory * UV2Factory::instance;

UV2Factory::UV2Factory()
{

}

UV2Factory:: ~UV2Factory()
{

}

UV2  * UV2Factory::CreatePermAttached(float * target)
{
	return new UV2(true, target);
}

void UV2Factory::CreateArray(int count, BaseVector2 *** ppptr)
{
	*ppptr = (BaseVector2**)new UV2*[count];
	if(*ppptr != NULL)
	{
		memset(*ppptr, (int)NULL, sizeof(UV2*) * count);
	}
}

UV2Factory * UV2Factory::GetInstance()
{
	if(instance == NULL)
	{
		instance = new UV2Factory();
	}

	return instance;
}
