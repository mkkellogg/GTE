#include "vector3array.h"
#include "vector3.h"
#include "vector3factory.h"
#include "base/basevector4array.h"
#include "debug/gtedebug.h"

namespace GTE
{
	Vector3Array::Vector3Array() : BaseVector4Array(Vector3Factory::Instance())
	{

	}

	Vector3Array::~Vector3Array()
	{

	}

	void Vector3Array::SetData(const Real * data, Bool includeW)
	{

	}

	Vector3 * Vector3Array::GetVector(Int32 index)
	{
		return (Vector3*)objects[index];
	}

	const Vector3 * Vector3Array::GetVectorConst(Int32 index) const
	{
		return (const Vector3*)objects[index];
	}

	Vector3 ** Vector3Array::GetVectors()
	{
		return (Vector3**)objects;
	}
}
