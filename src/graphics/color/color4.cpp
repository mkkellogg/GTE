#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "color4.h"

namespace GTE
{
	Color4::Color4() : BaseVector4(), r(data[0]), g(data[1]), b(data[2]), a(data[3])
	{

	}

	Color4::Color4(Bool permAttached, Real * target) : BaseVector4(permAttached, target), r(data[0]), g(data[1]), b(data[2]), a(data[3])
	{


	}

	Color4::Color4(Real r, Real g, Real b, Real a) : BaseVector4(r, g, b, a), r(data[0]), g(data[1]), b(data[2]), a(data[3])
	{

	}

	Color4::Color4(const Color4& color) : BaseVector4(color), r(data[0]), g(data[1]), b(data[2]), a(data[3])
	{

	}

	Color4::~Color4()
	{

	}

	/*
	 * Assignment operator
	 */
	Color4 & Color4::operator= (const Color4 & source)
	{
		if (this == &source)return *this;
		BaseVector4::operator=(source);
		return *this;
	}

	/*
	 * Over-ridden assignment operator from BaseVector4
	 */
	BaseVector4& Color4::operator= (const BaseVector4& source)
	{
		if (this == &source)return *this;
		BaseVector4::operator=(source);
		return *this;
	}

	void Color4::Set(Real r, Real g, Real b, Real a)
	{
		BaseVector4::Set(r, g, b, a);
	}
}
