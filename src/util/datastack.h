#ifndef _DATASTACK_
#define _DATASTACK_

template <typename T> class DataStack
{
	int maxEntryCount;
	int elementsPerEntry;
	int entries;
	T * data;

	T * stackPointer;

	public:

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

	}

	bool Init()
	{
		data = new T[maxEntryCount * elementsPerEntry];
		if(data == NULL)
		{
			return false;
		}

		stackPointer = data;

		return true;
	}

	void Push(const T * entryData)
	{
		if(data != NULL && stackPointer != NULL && entries < maxEntryCount)
		{
			memcpy(stackPointer, entryData, elementsPerEntry * sizeof(T));
			stackPointer += elementsPerEntry;
			entries++;
		}
	}

	/*
	 * !!! Important: The pointer returned from this method points to the data array for this stack. This
	 * means the data this pointer points to can be overwritten by Push() operations. Therefore the return
	 * value from this method is only guaranteed to be valid up until any subsequent Push() calls.
	 */
	T * Pop()
	{
		if(data != NULL && stackPointer != NULL && entries > 0)
		{
			stackPointer -= elementsPerEntry;
			entries--;
			return stackPointer;
		}

		return NULL;
	}

	int GetMaxEntryCount()
	{
		return maxEntryCount;
	}

	int GetEntryCount()
	{
		return entries;
	}
};

#endif
