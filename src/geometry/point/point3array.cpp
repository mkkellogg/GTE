#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "point3array.h"
#include "point3.h"
#include "point3factory.h"
#include "base/basevector4array.h"
#include "debug/gtedebug.h"

Point3Array::Point3Array() : BaseVector4Array(Point3Factory::Instance())
{
}

Point3Array::~Point3Array()
{
}

void Point3Array::SetData(const float * data, bool includeW)
{

}

Point3 * Point3Array::GetPoint(int index)
{
	return (Point3*)objects[index];
}

const Point3 * Point3Array::GetPointConst(int index) const
{
	return (const Point3*)objects[index];
}

Point3 ** Point3Array::GetPoints()
{
	return (Point3**)objects;
}
