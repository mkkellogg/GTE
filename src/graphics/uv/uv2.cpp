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



	/*
	* Override BaseVector2::AttachTo() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references u & v would point to invalid locations.
	*/
	void UV2::AttachTo(Real * data)
	{

	}

	/*
	* Override BaseVector2::Detach() and force it to DO NOTHING. If we allowed the backing storage
	* to change then the references u & v would point to invalid locations.
	*/
	void UV2::Detach()
	{

	}
}



