#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector2factory.h"
#include "basevector2.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	BaseVector2Factory::BaseVector2Factory()
	{

	}

	BaseVector2Factory::~BaseVector2Factory()
	{

	}

	BaseVector2 * BaseVector2Factory::CreatePermAttached(Real * target)
	{
		return new BaseVector2(true, target);
	}

	BaseVector2** BaseVector2Factory::CreateArray(Int32 count)
	{
		BaseVector2** pptr = new BaseVector2*[count];
		return pptr;
	}

	void BaseVector2Factory::DestroyArray(BaseVector2 ** array, UInt32 size)
	{
		NONFATAL_ASSERT(array != NULL, "BaseVector2Factory::DestroyArray -> 'array' is null.", true);

		for (UInt32 i = 0; i < size; i++)
		{
			BaseVector2 * baseObj = array[i];
			if (baseObj != NULL)
			{
				delete baseObj;
				array[i] = NULL;
			}
		}
		delete array;
	}
}