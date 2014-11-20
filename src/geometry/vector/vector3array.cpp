#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vector3array.h"
#include "vector3.h"
#include "vector3factory.h"
#include "base/basevector4array.h"
#include "ui/debug.h"

Vector3Array::Vector3Array() : BaseVector4Array(Vector3Factory::Instance())
{

}

Vector3Array::~Vector3Array()
{

}

void Vector3Array::SetData(const float * data, bool includeW)
{

}


Vector3 * Vector3Array::GetVector(int index)
{
	return (Vector3*)objects[index];
}

Vector3 ** Vector3Array::GetVectors()
{
	return (Vector3**)objects;
}
