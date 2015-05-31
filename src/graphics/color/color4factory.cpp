#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "color4factory.h"
#include "color4.h"
#include "base/basevector4.h"
#include "global/global.h"

namespace GTE
{
	Color4Factory * Color4Factory::instance;

	Color4Factory::Color4Factory()
	{

	}

	Color4Factory::~Color4Factory()
	{

	}

	BaseVector4 * Color4Factory::CreatePermAttached(Real * target)
	{
		return new Color4(true, target);
	}

	BaseVector4 ** Color4Factory::CreateArray(Int32 count)
	{
		BaseVector4** pptr = (BaseVector4**)new Color4*[count];
		ASSERT(pptr != NULL, "Color4Factory::CreateArray -> Unable to allocate array");

		memset(pptr, (Int32)NULL, sizeof(Color4*) * count);

		return pptr;
	}

	Color4Factory * Color4Factory::GetInstance()
	{
		if (instance == NULL)
		{
			instance = new Color4Factory();
		}

		return instance;
	}
}
