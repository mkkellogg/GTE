#ifndef _GTE_BASEVECTOR4ARRAY_H_
#define _GTE_BASEVECTOR4ARRAY_H_

#include "engine.h"
#include "basevector4.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class BaseVector4Factory;

	class BaseVector4Array
	{
	public:

		class Iterator
		{
			friend class BaseVector4Array;

		protected:

			BaseVector4Array& targetArray;
			BaseVector4& targetVector;
			Int32 currentIndex;

			Iterator(BaseVector4Array& targetArray, BaseVector4& targetVector) : targetArray(targetArray), targetVector(targetVector)
			{
				currentIndex = -1;
			}

		public:

			Bool HasNext()
			{
				return currentIndex < (Int32)targetArray.count - 1;
			}

			Bool MoveNext()
			{
				if (!HasNext())return false;
				currentIndex++;
				targetVector.AttachTo(targetArray.data + (currentIndex * 4));
				return true;
			}
		};

	protected:

		UInt32 reservedCount;
		UInt32 count;
		Real * data;
		BaseVector4 ** objects;
		BaseVector4Factory * baseFactory;

		void Destroy();

	public:

		BaseVector4Array(BaseVector4Factory * factory);
		virtual ~BaseVector4Array();

		BaseVector4 * GetBaseVector(UInt32 index);
		const Real * GetDataPtr() const;
		Bool Init(UInt32 reservedCount);
		UInt32 GetReservedCount() const;
		void SetCount(UInt32 count);
		UInt32 GetCount() const;
		Bool CopyTo(BaseVector4Array * dest) const;
		Iterator GetIterator(BaseVector4& targetVector);
	};
}

#endif
