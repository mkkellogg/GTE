#include "point3array.h"
#include "point3.h"
#include "base/basevector4factory.h"
#include "base/basevector4array.h"
#include "debug/gtedebug.h"

namespace GTE
{
	Point3Array::Point3Array() : BaseVector4Array<Point3>(new BaseVector4Factory<Point3>())
	{
	}

	Point3Array::~Point3Array()
	{
	}

	void Point3Array::SetData(const Real * data, Bool includeW)
	{

	}

	Point3 * Point3Array::GetPoint(Int32 index)
	{
		return (Point3*)objects[index];
	}

	const Point3 * Point3Array::GetPointConst(Int32 index) const
	{
		return (const Point3*)objects[index];
	}

	Point3 ** Point3Array::GetPoints()
	{
		return (Point3**)objects;
	}
}
