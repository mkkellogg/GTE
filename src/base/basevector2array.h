#ifndef _GTE_BASEVECTOR2ARRAY_H_
#define _GTE_BASEVECTOR2ARRAY_H_

#include "engine.h"
#include "basevector2.h"
#include "basevector2factory.h"
#include "global/global.h"

namespace GTE
{
	template <class T> class BaseVector2Array
	{
	protected:
		
		Int32 count;
		Real * data;
		T ** objects;
		BaseVector2Factory<T> * baseFactory;

		void Destroy()
		{
			if(objects != nullptr)baseFactory->DestroyArray(objects, count);
			objects = nullptr;

			SAFE_DELETE_ARRAY(data);
		}

	public:

		BaseVector2Array(BaseVector2Factory<T> * factory) : count(0), data(nullptr), objects(nullptr), baseFactory(factory)
		{

		}

		~BaseVector2Array()
		{
			Destroy();
		}

		Bool Init(Int32 count)
		{
			Destroy();

			this->count = count;

			data = new(std::nothrow) Real[count * 2];
			ASSERT(data != nullptr, "Could not allocate data memory for BaseVector2Array");

			objects = baseFactory->CreateArray(count);
			ASSERT(objects != nullptr, "Could not allocate objects memory for BaseVector2Array");

			Real *dataPtr = data;

			Int32 index = 0;
			while(index < count)
			{
				T * currentObject = (T*)baseFactory->CreatePermAttached(dataPtr);

				ASSERT(currentObject != nullptr, "Could not allocate BaseVector2 for BaseVector2Array");

				objects[index] = currentObject;

				dataPtr[0] = 0;
				dataPtr[1] = 0;

				dataPtr += 2;
				index++;
			}

			return true;
		}

		const Real * GetDataPtr() const
		{
			return (const Real *)data;
		}
	};
}

#endif
