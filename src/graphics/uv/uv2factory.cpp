#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uv2factory.h"
#include "uv2.h"
#include "base/basevector2.h"
#include "global/global.h"

namespace GTE
{
	UV2Factory * UV2Factory::instance;

	UV2Factory::UV2Factory()
	{

	}

	UV2Factory:: ~UV2Factory()
	{

	}

	UV2  * UV2Factory::CreatePermAttached(Real * target)
	{
		return new UV2(true, target);
	}

	BaseVector2** UV2Factory::CreateArray(Int32 count)
	{
		BaseVector2** pptr = (BaseVector2**)new UV2*[count];
		ASSERT(pptr != NULL, "UV2Factory::CreateArray -> Could not allocate array.");

		memset(pptr, (Int32)NULL, sizeof(UV2*) * count);

		return pptr;
	}

	UV2Factory * UV2Factory::GetInstance()
	{
		if (instance == NULL)
		{
			instance = new UV2Factory();
		}

		return instance;
	}
}
