#ifndef _GTE_VECTOR3_FACTORY_H_
#define _GTE_VECTOR3_FACTORY_H_

// forward declarations
class Vector3;

#include "base/basevector4factory.h"

class Vector3Factory : public BaseVector4Factory
{
	static Vector3Factory * instance;

    public:

	static Vector3Factory * Instance();

	Vector3Factory();
    virtual ~Vector3Factory();

    BaseVector4 * CreatePermAttached(float * target);
    BaseVector4 ** CreateArray(int count);
};

#endif
