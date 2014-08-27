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

	void Push(T * entryData)
	{
		if(entries < maxEntryCount)
		{
			memcpy(stackPointer, entryData, elementsPerEntry * sizeof(T));
			stackPointer += elementsPerEntry;
			entries++;
		}
	}

	T * Pop()
	{
		if(entries > 0)
		{
			T * ptr = stackPointer;
			stackPointer -= elementsPerEntry;
			entries--;
			return ptr;
		}

		return NULL;
	}
};

#endif
