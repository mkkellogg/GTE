#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "point3factory.h"
#include "point3.h"
#include "base/basevector4.h"
#include "global/global.h"

namespace GTE
{
	Point3Factory * Point3Factory::instance;

	Point3Factory::Point3Factory()
	{

	}

	Point3Factory:: ~Point3Factory()
	{

	}

	BaseVector4 * Point3Factory::CreatePermAttached(Real * target)
	{
		return new Point3(true, target);
	}

	BaseVector4** Point3Factory::CreateArray(int count)
	{
		BaseVector4** pptr = (BaseVector4**)new Point3*[count];
		ASSERT(pptr != NULL, "Point3Factory::CreateArray -> Could not allocate new array.");

		memset(pptr, (int)NULL, sizeof(Point3*) * count);

		return pptr;
	}

	Point3Factory * Point3Factory::Instance()
	{
		if (instance == NULL)
		{
			instance = new Point3Factory();
		}

		return instance;
	}
}
