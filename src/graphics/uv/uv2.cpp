#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "uv2.h"

namespace GTE
{
	UV2::UV2() : BaseVector2(), u(data[0]), v(data[1])
	{

	}

	UV2::UV2(Bool permAttached, Real * target) : BaseVector2(permAttached, target), u(data[0]), v(data[1])
	{


	}

	UV2::UV2(Real u, Real v) : BaseVector2(u, v), u(data[0]), v(data[1])
	{

	}

	UV2::UV2(const UV2& uv) : BaseVector2(uv), u(data[0]), v(data[1])
	{

	}

	UV2::~UV2()
	{

	}

	/*
	 * Assignment operator
	 */
	UV2 & UV2::operator= (const UV2 & source)
	{
		if (this == &source)return *this;
		BaseVector2::operator=(source);
		return *this;
	}

	/*
	 * Over-ridden assignment operator from BaseVector2
	 */
	BaseVector2& UV2::operator= (const BaseVector2& source)
	{
		if (this == &source)return *this;
		BaseVector2::operator=(source);
		return *this;
	}

	void UV2::AttachTo(Real * data)
	{
		BaseVector2::AttachTo(data);
	}

	void UV2::Detach()
	{
		BaseVector2::Detach();
	}
}



