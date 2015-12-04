#include "color4array.h"
#include "color4.h"
#include "base/basevector4factory.h"
#include "base/basevector4array.h"
#include "debug/gtedebug.h"

namespace GTE
{
	Color4Array::Color4Array() : BaseVector4Array<Color4>(new BaseVector4Factory<Color4>())
	{
	}

	Color4Array::~Color4Array()
	{
	}

	void Color4Array::SetData(const Real * data, Bool includeW)
	{

	}


	Color4 * Color4Array::GetColor(Int32 index)
	{
		return (Color4*)objects[index];
	}

	Color4 ** Color4Array::GetColors()
	{
		return (Color4**)objects;
	}
}
