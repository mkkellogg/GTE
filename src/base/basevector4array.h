#ifndef _GTE_BASEVECTOR4ARRAY_H_
#define _GTE_BASEVECTOR4ARRAY_H_

#include "engine.h"
#include "basevector4.h"
#include "basevector4factory.h"
#include "global/global.h"

namespace GTE
{
	template <class T> class BaseVector4Array
	{
	public:

		class Iterator
		{
			friend class BaseVector4Factory<T>;

		protected:

			BaseVector4Array<T>& targetArray;
			T& targetVector;
			Int32 currentIndex;

			Iterator(BaseVector4Array<T>& targetArray, T& targetVector) : targetArray(targetArray), targetVector(targetVector)
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
		T ** objects;
		BaseVector4Factory<T> * baseFactory;

		void Destroy()
		{
			if(objects != nullptr)baseFactory->DestroyArray(objects, reservedCount);
			objects = nullptr;

			SAFE_DELETE_ARRAY(data);
		}

	public:

		BaseVector4Array() : BaseVector4Array(new BaseVector4Factory<T>())
		{

		}

		BaseVector4Array(BaseVector4Factory<T> * factory) : reservedCount(0), count(0), data(nullptr), objects(nullptr), baseFactory(factory)
		{

		}

		virtual ~BaseVector4Array()
		{
			Destroy();
		}

		Bool Init(UInt32 reservedCount)
		{
			Destroy();

			this->reservedCount = reservedCount;
			this->count = reservedCount;

			data = new(std::nothrow) Real[reservedCount * 4];
			ASSERT(data != nullptr, "Could not allocate data memory for BaseVector4Array");

			objects = baseFactory->CreateArray(reservedCount);
			ASSERT(objects != nullptr, "Could not allocate objects memory for BaseVector4Array");

			Real *dataPtr = data;

			UInt32 index = 0;
			while(index < reservedCount)
			{
				T * currentObject = (T*)baseFactory->CreatePermAttached(dataPtr);
				ASSERT(currentObject != nullptr, "Could not allocate BaseVector4 for BaseVector4Array");

				objects[index] = currentObject;

				dataPtr[0] = 0;
				dataPtr[1] = 0;
				dataPtr[2] = 0;
				dataPtr[3] = 0;

				dataPtr += 4;
				index++;
			}

			return true;
		}

		T * GetBaseVector(UInt32 index)
		{
			NONFATAL_ASSERT_RTRN(index < count, "BaseVector4Array::GetBaseVector -> Index is out of range.", nullptr, true);

			return objects[index];
		}

		const Real * GetDataPtr() const
		{
			return (const Real *)data;
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

		Bool CopyTo(BaseVector4Array<T> * dest) const
		{
			NONFATAL_ASSERT_RTRN(dest != nullptr, " BaseVector4Array::CopyTo -> 'dest' is null.", false, true);

			if(dest->GetCount() != count)
			{
				Debug::PrintError("BaseVector4Array::CopyTo -> Source count does not match destination count.");
				return false;
			}

			memcpy(dest->data, data, count * sizeof(Real) * 4);

			return true;
		}

		Iterator GetIterator(T& targetVector)
		{
			return Iterator(*this, targetVector);
		}
	};
}

#endif
