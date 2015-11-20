#include <stdio.h>
#include <stdlib.h>

#include "vector2array.h"
#include "vector2.h"
#include "vector2factory.h"
#include "base/basevector2array.h"
#include "debug/gtedebug.h"

namespace GTE
{
	Vector2Array::Vector2Array() : BaseVector2Array(Vector2Factory::GetInstance())
	{
	}

	Vector2Array::~Vector2Array()
	{
	}

	void Vector2Array::SetData(const Real * data)
	{

	}

	Vector2 * Vector2Array::GetCoordinate(Int32 index)
	{
		return (Vector2*)objects[index];
	}

	Vector2 ** Vector2Array::GetCoordinates()
	{
		return (Vector2**)objects;
	}
}
