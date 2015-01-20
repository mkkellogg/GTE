#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector2factory.h"
#include "basevector2.h"
#include "global/global.h"
#include "debug/gtedebug.h"

BaseVector2Factory::BaseVector2Factory()
{

}

BaseVector2Factory::~BaseVector2Factory()
{

}

BaseVector2 * BaseVector2Factory::CreatePermAttached(float * target)
{
	return new BaseVector2(true, target);
}

BaseVector2** BaseVector2Factory::CreateArray(int count)
{
	BaseVector2** pptr = new BaseVector2*[count];
	return pptr;
}

void BaseVector2Factory::DestroyArray(BaseVector2 ** array, unsigned int size)
{
	ASSERT_RTRN(array != NULL, "BaseVector2Factory::DestroyArray -> array is NULL.");

	for(unsigned int i=0; i < size; i++)
	{
		BaseVector2 * baseObj = array[i];
		if(baseObj != NULL)
		{
			delete baseObj;
			array[i] = NULL;
		}
	}
	delete array;
}
