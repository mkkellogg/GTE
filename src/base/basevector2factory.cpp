#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector2factory.h"
#include "basevector2.h"

BaseVector2Factory::BaseVector2Factory()
{

}

BaseVector2Factory::~BaseVector2Factory()
{

}

BaseVector2 * BaseVector2Factory::CreatePermAttached(float * target)
{
	return new BaseVector2(true, target);
}

void BaseVector2Factory::CreateArray(int count, BaseVector2*** ppptr)
{
	*ppptr = new BaseVector2*[count];
}
