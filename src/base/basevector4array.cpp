#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector4array.h"
#include "basevector4.h"
#include "basevector4factory.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	BaseVector4Array::BaseVector4Array(BaseVector4Factory * factory) : reservedCount(0), count(0), data(NULL), objects(NULL), baseFactory(factory)
	{

	}

	BaseVector4Array::~BaseVector4Array()
	{
		Destroy();
	}

	void BaseVector4Array::Destroy()
	{
		if (objects != NULL)baseFactory->DestroyArray(objects, reservedCount);
		objects = NULL;

		SAFE_DELETE(data);
	}

	bool BaseVector4Array::Init(unsigned int reservedCount)
	{
		Destroy();

		this->reservedCount = reservedCount;
		this->count = reservedCount;

		data = new float[reservedCount * 4];
		ASSERT(data != NULL, "Could not allocate data memory for BaseVector4Array");

		objects = baseFactory->CreateArray(reservedCount);
		ASSERT(objects != NULL, "Could not allocate objects memory for BaseVector4Array");

		float *dataPtr = data;

		unsigned int index = 0;
		while (index < reservedCount)
		{
			BaseVector4 * currentObject = (BaseVector4*)baseFactory->CreatePermAttached(dataPtr);
			ASSERT(currentObject != NULL, "Could not allocate BaseVector4 for BaseVector4Array");

			objects[index] = currentObject;
			currentObject->Set(0, 0, 0, 0);

			dataPtr += 4;
			index++;
		}

		return true;
	}

	BaseVector4 * BaseVector4Array::GetBaseVector(unsigned int index)
	{
		NONFATAL_ASSERT_RTRN(index < count, "BaseVector4Array::GetBaseVector -> Index is out of range.", NULL, true);

		return objects[index];
	}

	const float * BaseVector4Array::GetDataPtr() const
	{
		return (const float *)data;
	}

	unsigned int BaseVector4Array::GetReservedCount() const
	{
		return reservedCount;
	}

	void BaseVector4Array::SetCount(unsigned int count)
	{
		this->count = count;
	}

	unsigned int BaseVector4Array::GetCount() const
	{
		return count;
	}

	bool BaseVector4Array::CopyTo(BaseVector4Array * dest) const
	{
		NONFATAL_ASSERT_RTRN(dest != NULL, " BaseVector4Array::CopyTo -> 'dest' is null.", false, true);

		if (dest->GetCount() != count)
		{
			Debug::PrintError("BaseVector4Array::CopyTo -> Source count does not match destination count.");
			return false;
		}

		memcpy(dest->data, data, count * sizeof(float) * 4);

		return true;
	}

	BaseVector4Array::Iterator BaseVector4Array::GetIterator(BaseVector4& targetVector)
	{
		return Iterator(*this, targetVector);
	}
}
