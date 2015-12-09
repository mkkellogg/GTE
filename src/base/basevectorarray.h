#ifndef _GTE_BASEVECTORARRAY_H_
#define _GTE_BASEVECTORARRAY_H_

#include "engine.h"
#include "basevectorfactory.h"
#include "global/global.h"

namespace GTE
{
	template <class T> class BaseVectorArray
	{
	public:

		class Iterator
		{
			friend class BaseVectorFactory<T>;

		protected:

			BaseVectorArray<T>& targetArray;
			T& targetVector;
			Int32 currentIndex;

			Iterator(BaseVectorArray<T>& targetArray, T& targetVector) : targetArray(targetArray), targetVector(targetVector)
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
				targetVector.AttachTo(targetArray.data + (currentIndex * BaseVectorTraits<T>::VectorSize));
				return true;
			}
		};

	protected:

		UInt32 reservedCount;
		UInt32 count;
		Real * data;
		T ** objects;
		BaseVectorFactory<T> * baseFactory;

		void Destroy()
		{
			if(objects != nullptr)baseFactory->DestroyArray(objects, reservedCount);
			objects = nullptr;

			SAFE_DELETE_ARRAY(data);
		}

	public:

		BaseVectorArray() : BaseVectorArray(new BaseVectorFactory<T>())
		{

		}

		BaseVectorArray	(BaseVectorFactory<T> * factory) : reservedCount(0), count(0), data(nullptr), objects(nullptr), baseFactory(factory)
		{

		}

		virtual ~BaseVectorArray()
		{
			Destroy();
		}

		Bool Init(UInt32 reservedCount)
		{
			Destroy();

			this->reservedCount = reservedCount;
			this->count = reservedCount;

			data = new(std::nothrow) Real[reservedCount * BaseVectorTraits<T>::VectorSize];
			ASSERT(data != nullptr, "Could not allocate data memory for BaseVectorArray");

			objects = baseFactory->CreateArray(reservedCount);
			ASSERT(objects != nullptr, "Could not allocate objects memory for BaseVectorArray");

			Real *dataPtr = data;

			UInt32 index = 0;
			while(index < reservedCount)
			{
				T * currentObject = (T*)baseFactory->CreatePermAttached(dataPtr);
				ASSERT(currentObject != nullptr, "Could not allocate element for BaseVectorArray");

				objects[index] = currentObject;

				for(UInt32 i = 0; i < BaseVectorTraits<T>::VectorSize; i++)
				{
					dataPtr[i] = 0;
				}

				dataPtr += BaseVectorTraits<T>::VectorSize;
				index++;
			}

			return true;
		}

		T * GetBaseVector(UInt32 index)
		{
			NONFATAL_ASSERT_RTRN(index < count, "BaseVectorArray::GetBaseVector -> Index is out of range.", nullptr, true);

			return objects[index];
		}

		Real * GetDataPtr()
		{
			return data;
		}

		const Real * GetConstDataPtr() const
		{
			return data;
		}

		T * GetElement(Int32 index)
		{
			return objects[index];
		}

		const T * GetElementConst(Int32 index) const
		{
			return (const T*)objects[index];
		}

		T ** GetElements()
		{
			return objects;
		}

		UInt32 GetReservedCount() const
		{
			return reservedCount;
		}

		void SetCount(UInt32 count)
		{
			this->count = count;
		}

		UInt32 GetCount() const
		{
			return count;
		}

		Bool CopyTo(BaseVectorArray<T> * dest) const
		{
			NONFATAL_ASSERT_RTRN(dest != nullptr, " BaseVectorArray::CopyTo -> 'dest' is null.", false, true);

			if(dest->GetCount() != count)
			{
				Debug::PrintError("BaseVectorArray::CopyTo -> Source count does not match destination count.");
				return false;
			}

			memcpy(dest->data, data, count * sizeof(Real) * BaseVectorTraits<T>::VectorSize);

			return true;
		}

		Iterator GetIterator(T& targetVector)
		{
			return Iterator(*this, targetVector);
		}
	};
}

#endif
