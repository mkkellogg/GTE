#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "color4factory.h"
#include "color4.h"
#include "base/basevector4.h"

Color4Factory * Color4Factory::instance;

Color4Factory::Color4Factory()
{

}

Color4Factory::~Color4Factory()
{

}

BaseVector4 * Color4Factory::CreatePermAttached(float * target)
{
	return new Color4(true, target);
}

BaseVector4 ** Color4Factory::CreateArray(int count)
{
	BaseVector4** pptr = (BaseVector4**)new Color4*[count];
	if(pptr != NULL)
	{
		memset(pptr, (int)NULL, sizeof(Color4*) * count);
	}
	return pptr;
}

Color4Factory * Color4Factory::GetInstance()
{
	if(instance == NULL)
	{
		instance = new Color4Factory();
	}

	return instance;
}
