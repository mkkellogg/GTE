#ifndef _COLOR4_FACTORY_H_
#define _COLOR4_FACTORY_H_

//forward declarations
class Color4Factory;
class Color4;

#include "base/basevector4factory.h"

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

#endif
