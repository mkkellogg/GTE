#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vector2factory.h"
#include "vector2.h"
#include "base/basevector2.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE
{
	Vector2Factory * Vector2Factory::instance;

	Vector2Factory::Vector2Factory()
	{
	}

	Vector2Factory:: ~Vector2Factory()
	{
	}

	Vector2  * Vector2Factory::CreatePermAttached(Real * target)
	{
		return new Vector2(true, target);
	}

	BaseVector2** Vector2Factory::CreateArray(Int32 count)
	{
		BaseVector2** pptr = (BaseVector2**)new(std::nothrow)  Vector2*[count];
		ASSERT(pptr != nullptr, "Vector2Factory::CreateArray -> Could not allocate array.");

		memset(pptr, (Int32)NULL, sizeof(Vector2*) * count);

		return pptr;
	}

	Vector2Factory * Vector2Factory::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new(std::nothrow)  Vector2Factory();
		}

		return instance;
	}
}
