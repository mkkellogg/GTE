#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uv2array.h"
#include "uv2.h"
#include "uv2factory.h"
#include "base/basevector2array.h"
#include "ui/debug.h"

UV2Array::UV2Array(int count) : BaseVector2Array(count, UV2Factory::GetInstance())
{
}

UV2Array::~UV2Array()
{
}

void UV2Array::SetData(const float * data)
{

}

UV2 * UV2Array::GetCoordinate(int index)
{
	return (UV2*)objects[index];
}

UV2 ** UV2Array::GetCoordinates()
{
	return (UV2**)objects;
}
