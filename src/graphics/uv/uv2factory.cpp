#include <memory.h>

#include "uv2factory.h"
#include "uv2.h"
#include "base/basevector2.h"
#include "global/global.h"
#include "global/assert.h"

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
		BaseVector2** pptr = (BaseVector2**)new(std::nothrow)  UV2*[count];
		ASSERT(pptr != nullptr, "UV2Factory::CreateArray -> Could not allocate array.");

		memset(pptr, (Int32)NULL, sizeof(UV2*) * count);

		return pptr;
	}

	UV2Factory * UV2Factory::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new(std::nothrow)  UV2Factory();
		}

		return instance;
	}
}
