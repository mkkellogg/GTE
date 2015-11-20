#include <stdio.h>
#include <stdlib.h>

#include "basevector4array.h"
#include "basevector4.h"
#include "basevector4factory.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"

namespace GTE
{
	BaseVector4Array::BaseVector4Array(BaseVector4Factory * factory) : reservedCount(0), count(0), data(nullptr), objects(nullptr), baseFactory(factory)
	{

	}

	BaseVector4Array::~BaseVector4Array()
	{
		Destroy();
	}

	void BaseVector4Array::Destroy()
	{
		if (objects != nullptr)baseFactory->DestroyArray(objects, reservedCount);
		objects = nullptr;

		SAFE_DELETE(data);
	}

	Bool BaseVector4Array::Init(UInt32 reservedCount)
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
		while (index < reservedCount)
		{
			BaseVector4 * currentObject = (BaseVector4*)baseFactory->CreatePermAttached(dataPtr);
			ASSERT(currentObject != nullptr, "Could not allocate BaseVector4 for BaseVector4Array");

			objects[index] = currentObject;
			currentObject->Set(0, 0, 0, 0);

			dataPtr += 4;
			index++;
		}

		return true;
	}

	BaseVector4 * BaseVector4Array::GetBaseVector(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < count, "BaseVector4Array::GetBaseVector -> Index is out of range.", nullptr, true);

		return objects[index];
	}

	const Real * BaseVector4Array::GetDataPtr() const
	{
		return (const Real *)data;
	}

	UInt32 BaseVector4Array::GetReservedCount() const
	{
		return reservedCount;
	}

	void BaseVector4Array::SetCount(UInt32 count)
	{
		this->count = count;
	}

	UInt32 BaseVector4Array::GetCount() const
	{
		return count;
	}

	Bool BaseVector4Array::CopyTo(BaseVector4Array * dest) const
	{
		NONFATAL_ASSERT_RTRN(dest != nullptr, " BaseVector4Array::CopyTo -> 'dest' is null.", false, true);

		if (dest->GetCount() != count)
		{
			Debug::PrintError("BaseVector4Array::CopyTo -> Source count does not match destination count.");
			return false;
		}

		memcpy(dest->data, data, count * sizeof(Real) * 4);

		return true;
	}

	BaseVector4Array::Iterator BaseVector4Array::GetIterator(BaseVector4& targetVector)
	{
		return Iterator(*this, targetVector);
	}
}
