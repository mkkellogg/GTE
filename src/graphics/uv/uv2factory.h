#ifndef _GTE_UV2_FACTORY_H_
#define _GTE_UV2_FACTORY_H_

#include "base/basevector2factory.h"
#include "uv2.h"

namespace GTE
{
	class UV2Factory : public BaseVector2Factory
	{
		static UV2Factory * instance;

	public:

		static UV2Factory * GetInstance();

		UV2Factory();
		virtual ~UV2Factory();

		virtual UV2 * CreatePermAttached(Real * target);
		virtual BaseVector2** CreateArray(Int32 count);
	};
}

#endif
