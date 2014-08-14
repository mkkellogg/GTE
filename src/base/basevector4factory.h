#ifndef _BASEVECTOR4_FACTORY_H_
#define _BASEVECTOR4_FACTORY_H_

#include "basevector4.h"

class BaseVector4Factory
{
    public:

	BaseVector4Factory();
    virtual ~BaseVector4Factory();

    virtual BaseVector4 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector4*** ppptr);
};

#endif
