#ifndef _GTE_BASEVECTOR2_FACTORY_H_
#define _GTE_BASEVECTOR2_FACTORY_H_

#include "engine.h"
#include "global/global.h"
#include "global/assert.h"
#include "basevector2.h"

#include <memory.h>

namespace GTE
{
	template <class T> class BaseVector2Factory
	{
	public:

		BaseVector2Factory()
		{

		}

		virtual ~BaseVector2Factory()
		{

		}

		T * CreatePermAttached(Real * target)
		{
			return new(std::nothrow) T(true, target);
		}

		T** CreateArray(Int32 count)
		{
			T** pptr = new(std::nothrow) T*[count];
			return pptr;
		}

		void DestroyArray(T ** array, UInt32 size)
		{
			NONFATAL_ASSERT(array != nullptr, "BaseVector2Factory::DestroyArray -> 'array' is null.", true);

			for(UInt32 i = 0; i < size; i++)
			{
				T * baseObj = array[i];
				if(baseObj != nullptr)
				{
					delete baseObj;
					array[i] = nullptr;
				}
			}
			delete[] array;
		}
	};
}

#endif
