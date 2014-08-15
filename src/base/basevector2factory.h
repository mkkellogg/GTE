#ifndef _BASEVECTOR2_FACTORY_H_
#define _BASEVECTOR2_FACTORY_H_

#include "basevector2.h"

class BaseVector2Factory
{
    public:

	BaseVector2Factory();
    virtual ~BaseVector2Factory();

    virtual BaseVector2 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector2*** ppptr);
};

#endif
