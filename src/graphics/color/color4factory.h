#ifndef _GTE_COLOR4_FACTORY_H_
#define _GTE_COLOR4_FACTORY_H_

#include "base/basevector4factory.h"

namespace GTE
{
	//forward declarations
	class Color4Factory;
	class Color4;

	class Color4Factory : public BaseVector4Factory
	{
		static Color4Factory * instance;

	public:

		static Color4Factory * GetInstance();

		Color4Factory();
		virtual ~Color4Factory();

		virtual BaseVector4 * CreatePermAttached(float * target);
		virtual BaseVector4 ** CreateArray(int count);
	};
}

#endif
