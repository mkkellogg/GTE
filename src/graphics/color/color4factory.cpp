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

void Color4Factory::CreateArray(int count, BaseVector4 *** ppptr)
{
	*ppptr = (BaseVector4**)new Color4*[count];
	if(*ppptr != NULL)
	{
		memset(*ppptr, (int)NULL, sizeof(Color4*) * count);
	}
}

Color4Factory * Color4Factory::GetInstance()
{
	if(instance == NULL)
	{
		instance = new Color4Factory();
	}

	return instance;
}
