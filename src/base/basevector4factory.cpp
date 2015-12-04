#include <memory.h>

#include "basevector4factory.h"
#include "basevector4.h"
#include "global/global.h"
#include "global/assert.h"
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
		return new(std::nothrow) BaseVector4(true, target);
	}

	BaseVector4** BaseVector4Factory::CreateArray(Int32 count)
	{
		BaseVector4** pptr = new(std::nothrow) BaseVector4*[count];
		return pptr;
	}

	void BaseVector4Factory::DestroyArray(BaseVector4 ** array, UInt32 size)
	{
		NONFATAL_ASSERT(array != nullptr, "BaseVector4Factory::DestroyArray -> 'array' is null.", true);

		for (UInt32 i = 0; i < size; i++)
		{
			BaseVector4 * baseObj = array[i];
			if (baseObj != nullptr)
			{
				delete baseObj;
				array[i] = nullptr;
			}
		}
		delete[] array;
	}
}

