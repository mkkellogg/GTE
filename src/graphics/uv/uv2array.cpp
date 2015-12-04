#include "uv2array.h"
#include "uv2.h"
#include "base/basevector2factory.h"
#include "base/basevector2array.h"
#include "debug/gtedebug.h"

namespace GTE
{
	UV2Array::UV2Array() : BaseVector2Array<UV2>(new BaseVector2Factory<UV2>())
	{
	}

	UV2Array::~UV2Array()
	{
	}

	void UV2Array::SetData(const Real * data)
	{

	}

	UV2 * UV2Array::GetCoordinate(Int32 index)
	{
		return (UV2*)objects[index];
	}

	UV2 ** UV2Array::GetCoordinates()
	{
		return (UV2**)objects;
	}
}
