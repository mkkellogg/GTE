#ifndef _COLOR4_FACTORY_H_
#define _COLOR4_FACTORY_H_

#include "base/basevector4factory.h"
#include "base/basevector4.h"
#include "color4.h"

class Color4Factory : public BaseVector4Factory
{
	static Color4Factory * instance;

    public:

	static Color4Factory * GetInstance();

	Color4Factory();
    virtual ~Color4Factory();

    virtual Color4 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector4 *** ppptr);
};

#endif
