#ifndef _VECTOR3_FACTORY_H_
#define _VECTOR3_FACTORY_H_

#include "base/basevector4factory.h"
#include "vector3.h"

class Vector3Factory : public BaseVector4Factory
{
	static Vector3Factory * instance;

    public:

	static Vector3Factory * GetInstance();

	Vector3Factory();
    virtual ~Vector3Factory();

    virtual Vector3 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector4 *** ppptr);
};

#endif
