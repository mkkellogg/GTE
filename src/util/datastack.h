#ifndef _GTE_DATASTACK_
#define _GTE_DATASTACK_

#include "object/enginetypes.h"
#include "global/global.h"

namespace GTE
{
	template <typename T> class DataStack
	{
		UInt32 maxEntryCount;
		UInt32 elementsPerEntry;
		UInt32 entries;
		T * data;
		T * stackPointer;

		void Destroy()
		{
			if (data != NULL)
			{
				delete[] data;
				data = NULL;
			}
		}

	public:

		DataStack()
		{
			this->maxEntryCount = 256;
			this->elementsPerEntry = 1;
			entries = 0;
			data = NULL;
			stackPointer = NULL;
		}

		DataStack(Int32 maxEntryCount, Int32 elementsPerEntry)
		{
			this->maxEntryCount = maxEntryCount;
			this->elementsPerEntry = elementsPerEntry;
			entries = 0;
			data = NULL;
			stackPointer = NULL;
		}

		~DataStack()
		{
			Destroy();
		}

		Bool Init()
		{
			Destroy();

			data = new T[maxEntryCount * elementsPerEntry];
			if (data == NULL)
			{
				return false;
			}

			stackPointer = data;

			return true;
		}

		void Push(const T * entryData)
		{
			if (data != NULL && stackPointer != NULL && entries < maxEntryCount)
			{
				memcpy(stackPointer, entryData, elementsPerEntry * sizeof(T));
				stackPointer += elementsPerEntry;
				entries++;
			}
		}

		/*
		 * !!! IMPORTANT: The pointer returned from this method points to the data array for this stack. This
		 * means the data this pointer points to can be overwritten by Push() operations. Therefore the return
		 * value from this method is only guaranteed to be valid up until any subsequent Push() calls.
		 */
		T * Pop()
		{
			if (data != NULL && stackPointer != NULL && entries > 0)
			{
				stackPointer -= elementsPerEntry;
				entries--;
				return stackPointer;
			}

			return NULL;
		}

		Int32 GetMaxEntryCount() const
		{
			return maxEntryCount;
		}

		UInt32 GetEntryCount() const
		{
			return entries;
		}
	};
}

#endif
