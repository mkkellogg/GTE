#include "basevector4.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

namespace GTE
{
	/*
* Initialize - Store [x], [y], [z], and [w] as the base data.
*
* If [target] is not null, then use it as the storage array.
*
* If [permAttach] is true, then [target] cannot be null, as it will be used as the permanent
* backing storage array. If [permAttach] is false, then this base vector can attach and
* re-attach to various different backing storage arrays.
*/
	void BaseVector4::Init(Real *target, Bool permAttach)
	{
		data = baseData;

		if (target == nullptr && permAttach == true)
		{
			Debug::PrintWarning("BaseVector4::Init -> 'permAttach' is true, but 'target' is null!");
			permAttach = false;
		}

		if (permAttach)
		{
			canDetach = false;
		}
		else
		{
			data = baseData;
			canDetach = true;
		}

		if (target != nullptr)
		{
			data = target;
			attached = true;
		}
		else
		{
			attached = false;
		}
	}

	/*
	 * Default constructor
	 */
	BaseVector4::BaseVector4()
	{
		Init(nullptr, false);
		Set(0, 0, 0, 0);
	}

	/*
	 * Constructor will alternate backing storage
	 */
	BaseVector4::BaseVector4(Bool permAttached, Real * target)
	{
		Init(target, true);
	}

	/*
	 * Constructor with initialization values
	 */
	BaseVector4::BaseVector4(Real x, Real y, Real z, Real w)
	{
		Init(nullptr, false);
		Set(x, y, z, w);
	}

	/*
	 * Copy constructor
	 */
	BaseVector4::BaseVector4(const BaseVector4& baseVector)
	{
		Init(nullptr, false);
		Set(baseVector.data[0], baseVector.data[1], baseVector.data[2], baseVector.data[3]);
	}

	/*
	 * Clean up
	 */
	BaseVector4::~BaseVector4()
	{
		if (!attached)data = nullptr;
	}

	/*
	 * Assignment operator.
	 */
	BaseVector4& BaseVector4::operator=(const BaseVector4& source)
	{
		if (this == &source)return *this;

		attached = source.attached;
		canDetach = source.canDetach;

		data[0] = source.data[0];
		data[1] = source.data[1];
		data[2] = source.data[2];
		data[3] = source.data[3];

		return *this;
	}

	/*
	 * Set this base vector's data
	 */
	void BaseVector4::Set(Real x, Real y, Real z, Real w)
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
	}

	/*
	 * Set this base vector's data to be equal to [baseVector]
	 */
	void BaseVector4::SetTo(const BaseVector4& baseVector)
	{
		data[0] = baseVector.data[0];
		data[1] = baseVector.data[1];
		data[2] = baseVector.data[2];
		data[3] = baseVector.data[3];
	}

	/*
	 * Copy this base vector's data into [baseVector]
	 */
	void BaseVector4::Get(BaseVector4& baseVector) const
	{
		memcpy(baseVector.data, data, sizeof(Real) * 4);
	}

	/*
	 * Get a pointer the backing data storage
	 */
	Real * BaseVector4::GetDataPtr()
	{
		return data;
	}

	/*
	 * Attach the base vector to a new backing storage array [data]
	 */
	void BaseVector4::AttachTo(Real * data)
	{
		this->data = data;
		attached = true;
	}

	/*
	 * Detach from the current backing storage array
	 */
	void BaseVector4::Detach()
	{
		if (canDetach)
		{
			this->data = baseData;
			attached = false;
		}
	}
}




