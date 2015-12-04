#include "uv2.h"
#include "base/basevector.h"

namespace GTE
{
	UV2::UV2() : BaseVector(), u(data[0]), v(data[1])
	{

	}

	UV2::UV2(Bool permAttached, Real * target) : BaseVector(permAttached, target), u(data[0]), v(data[1])
	{


	}

	UV2::UV2(Real u, Real v) : BaseVector(), u(data[0]), v(data[1])
	{
		Set(u, v);
	}

	UV2::UV2(const UV2& uv) : BaseVector(uv), u(data[0]), v(data[1])
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
		BaseVector::operator=(source);
		return *this;
	}

	/*
	 * Over-ridden assignment operator from BaseVector2
	 */
	BaseVector<UV2>& UV2::operator= (const BaseVector<UV2>& source)
	{
		if (this == &source)return *this;
		BaseVector::operator=(source);
		return *this;
	}

	void UV2::Set(Real u, Real v)
	{
		this->data[0] = u;
		this->data[1] = v;
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



