#ifndef _GTE_POINT3_FACTORY_H_
#define _GTE_POINT3_FACTORY_H_

#include "base/basevector4factory.h"

namespace GTE
{
	//forward declarations
	class Point3;

	class Point3Factory : public BaseVector4Factory
	{
		static Point3Factory * instance;

	public:

		static Point3Factory * Instance();

		Point3Factory();
		~Point3Factory() override;

		BaseVector4 * CreatePermAttached(Real * target) override;
		BaseVector4** CreateArray(Int32 count) override;
	};
}

#endif
