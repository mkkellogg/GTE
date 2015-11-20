#ifndef _GTE_BASEVECTOR4_FACTORY_H_
#define _GTE_BASEVECTOR4_FACTORY_H_

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	// forward declarations
	class BaseVector4;

	class BaseVector4Factory
	{
	public:

		BaseVector4Factory();
		virtual ~BaseVector4Factory();

		virtual BaseVector4 * CreatePermAttached(Real * target);
		virtual BaseVector4** CreateArray(Int32 count);
		virtual void DestroyArray(BaseVector4 ** array, UInt32 size);
	};
}

#endif
