#include "basevector2factory.h"
#include "basevector2.h"
#include "global/global.h"
#include "global/assert.h"
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
		return new(std::nothrow) BaseVector2(true, target);
	}

	BaseVector2** BaseVector2Factory::CreateArray(Int32 count)
	{
		BaseVector2** pptr = new(std::nothrow) BaseVector2*[count];
		return pptr;
	}

	void BaseVector2Factory::DestroyArray(BaseVector2 ** array, UInt32 size)
	{
		NONFATAL_ASSERT(array != nullptr, "BaseVector2Factory::DestroyArray -> 'array' is null.", true);

		for (UInt32 i = 0; i < size; i++)
		{
			BaseVector2 * baseObj = array[i];
			if (baseObj != nullptr)
			{
				delete baseObj;
				array[i] = nullptr;
			}
		}
		delete array;
	}
}
