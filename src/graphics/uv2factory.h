#ifndef _UV2_FACTORY_H_
#define _UV2_FACTORY_H_

#include "base/basevector2factory.h"
#include "uv2.h"

class UV2Factory : public BaseVector2Factory
{
	static UV2Factory * instance;

    public:

	static UV2Factory * GetInstance();

	UV2Factory();
    virtual ~UV2Factory();

    virtual UV2 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector2 *** ppptr);
};

#endif
