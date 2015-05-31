#ifndef _GTE_BASEVECTOR2_FACTORY_H_
#define _GTE_BASEVECTOR2_FACTORY_H_

#include "object/enginetypes.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class BaseVector2;

	class BaseVector2Factory
	{
	public:

		BaseVector2Factory();
		virtual ~BaseVector2Factory();

		virtual BaseVector2 * CreatePermAttached(Real * target);
		virtual BaseVector2 ** CreateArray(Int32 count);
		virtual void DestroyArray(BaseVector2 ** array, UInt32 size);
	};
}

#endif
