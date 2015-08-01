#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector2array.h"
#include "basevector2.h"
#include "basevector2factory.h"
#include "global/global.h"
#include "debug/gtedebug.h"

namespace GTE
{
	BaseVector2Array::BaseVector2Array(BaseVector2Factory * factory) : count(0), data(nullptr), objects(nullptr), baseFactory(factory)
	{

	}

	BaseVector2Array::~BaseVector2Array()
	{
		Destroy();
	}

	void BaseVector2Array::Destroy()
	{
		if (objects != nullptr)baseFactory->DestroyArray(objects, count);
		objects = nullptr;

		SAFE_DELETE(data);
	}

	Bool BaseVector2Array::Init(Int32 count)
	{
		Destroy();

		this->count = count;

		data = new(std::nothrow) Real[count * 2];
		ASSERT(data != nullptr, "Could not allocate data memory for BaseVector2Array");

		objects = baseFactory->CreateArray(count);
		ASSERT(objects != nullptr, "Could not allocate objects memory for BaseVector2Array");

		Real *dataPtr = data;

		Int32 index = 0;
		while (index < count)
		{
			BaseVector2 * currentObject = (BaseVector2*)baseFactory->CreatePermAttached(dataPtr);

			ASSERT(currentObject != nullptr, "Could not allocate BaseVector2 for BaseVector2Array");

			objects[index] = currentObject;
			currentObject->Set(0, 0);

			dataPtr += 2;
			index++;
		}

		return true;
	}

	const Real * BaseVector2Array::GetDataPtr() const
	{
		return (const Real *)data;
	}




}