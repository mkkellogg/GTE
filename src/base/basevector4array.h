#ifndef _BASEVECTOR4ARRAY_H_
#define _BASEVECTOR4ARRAY_H_

// forward declarations
class BaseVector4Factory;

#include "basevector4.h"

class BaseVector4Array
{
	public:

	class Iterator
	{
		friend class BaseVector4Array;

		protected:

		BaseVector4Array& targetArray;
		BaseVector4& targetVector;
		int currentIndex;

		Iterator(BaseVector4Array& targetArray, BaseVector4& targetVector) : targetArray(targetArray), targetVector(targetVector)
		{
			currentIndex = -1;
		}

		public:

		bool HasNext()
		{
			return currentIndex < (int)targetArray.usedCount - 1;
		}

		void Next()
		{
			if(currentIndex >= (int)targetArray.usedCount-1)return;
			currentIndex++;
			targetVector.AttachTo(targetArray.data + (currentIndex * 4));
		}
	};

	protected:

	unsigned int maxCount;
	unsigned int usedCount;
	float * data;
	BaseVector4 ** objects;
	BaseVector4Factory * baseFactory;

	void Destroy();

    public:

	BaseVector4Array(BaseVector4Factory * factory);
    virtual ~BaseVector4Array();

    const float * GetDataPtr() const;
    bool Init(unsigned int maxCount);
    unsigned int GetMaxCount();
    void SetUsedCount(unsigned int usedCount);
    unsigned int GetUsedCount();
    bool CopyTo(BaseVector4Array * dest) const;
    Iterator GetIterator(BaseVector4& targetVector);
};

#endif
