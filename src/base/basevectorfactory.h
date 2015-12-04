#ifndef _GTE_BASEVECTOR_FACTORY_H_
#define _GTE_BASEVECTOR_FACTORY_H_

#include "engine.h"
#include "global/global.h"
#include "global/assert.h"
#include "basevector.h"

#include <memory.h>

namespace GTE
{
	template <class T> class BaseVectorFactory
	{
	public:

	BaseVectorFactory()
		{

		}

		virtual ~BaseVectorFactory()
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
			NONFATAL_ASSERT(array != nullptr, "BaseVectorFactory::DestroyArray -> 'array' is null.", true);

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
