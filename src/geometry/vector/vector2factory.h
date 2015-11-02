#ifndef _GTE_UV2_FACTORY_H_
#define _GTE_UV2_FACTORY_H_

#include "base/basevector2factory.h"
#include "vector2.h"

namespace GTE
{
	class Vector2Factory : public BaseVector2Factory
	{
		static Vector2Factory * instance;

		public:

		static Vector2Factory * GetInstance();

		Vector2Factory();
		virtual ~Vector2Factory();

		virtual Vector2 * CreatePermAttached(Real * target);
		virtual BaseVector2** CreateArray(Int32 count);
	};
}

#endif
