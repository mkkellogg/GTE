#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vector3factory.h"
#include "vector3.h"
#include "base/basevector4.h"
#include "global/global.h"

namespace GTE
{
	Vector3Factory * Vector3Factory::instance;

	Vector3Factory::Vector3Factory()
	{

	}

	Vector3Factory::~Vector3Factory()
	{

	}

	BaseVector4 * Vector3Factory::CreatePermAttached(Real * target)
	{
		return new Vector3(true, target);
	}

	BaseVector4 ** Vector3Factory::CreateArray(int count)
	{
		BaseVector4** pptr = (BaseVector4**)new Vector3*[count];
		ASSERT(pptr != NULL, "Vector3Factory::CreateArray -> Could not allocate new array.");

		memset(pptr, (int)NULL, sizeof(Vector3*) * count);

		return pptr;
	}

	Vector3Factory * Vector3Factory::Instance()
	{
		if (instance == NULL)
		{
			instance = new Vector3Factory();
		}

		return instance;
	}
}