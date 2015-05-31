#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector4factory.h"
#include "basevector4.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	BaseVector4Factory::BaseVector4Factory()
	{

	}

	BaseVector4Factory::~BaseVector4Factory()
	{

	}

	BaseVector4 * BaseVector4Factory::CreatePermAttached(Real * target)
	{
		return new BaseVector4(true, target);
	}

	BaseVector4** BaseVector4Factory::CreateArray(int count)
	{
		BaseVector4** pptr = new BaseVector4*[count];
		return pptr;
	}

	void BaseVector4Factory::DestroyArray(BaseVector4 ** array, UInt32 size)
	{
		NONFATAL_ASSERT(array != NULL, "BaseVector4Factory::DestroyArray -> 'array' is null.", true);

		for (UInt32 i = 0; i < size; i++)
		{
			BaseVector4 * baseObj = array[i];
			if (baseObj != NULL)
			{
				delete baseObj;
				array[i] = NULL;
			}
		}
		delete array;
	}
}

