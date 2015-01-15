#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "color4array.h"
#include "color4.h"
#include "color4factory.h"
#include "base/basevector4array.h"
#include "debug/debug.h"

Color4Array::Color4Array() : BaseVector4Array(Color4Factory::GetInstance())
{
}

Color4Array::~Color4Array()
{
}

void Color4Array::SetData(const float * data, bool includeW)
{

}


Color4 * Color4Array::GetColor(int index)
{
	return (Color4*)objects[index];
}

Color4 ** Color4Array::GetColors()
{
	return (Color4**)objects;
}
