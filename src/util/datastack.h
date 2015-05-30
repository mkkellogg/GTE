#ifndef _GTE_DATASTACK_
#define _GTE_DATASTACK_

#include "global/global.h"

namespace GTE
{
	template <typename T> class DataStack
	{
		unsigned int maxEntryCount;
		unsigned int elementsPerEntry;
		unsigned int entries;
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

		DataStack(int maxEntryCount, int elementsPerEntry)
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

		bool Init()
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

		int GetMaxEntryCount() const
		{
			return maxEntryCount;
		}

		unsigned int GetEntryCount() const
		{
			return entries;
		}
	};
}

#endif
