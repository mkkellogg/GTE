#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "basevector4factory.h"
#include "basevector4.h"

BaseVector4Factory::BaseVector4Factory()
{

}

BaseVector4Factory::~BaseVector4Factory()
{

}

BaseVector4 * BaseVector4Factory::CreatePermAttached(float * target)
{
	return new BaseVector4(true, target);
}

void BaseVector4Factory::CreateArray(int count, BaseVector4*** ppptr)
{
	*ppptr = new BaseVector4*[count];
}