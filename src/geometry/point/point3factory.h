#ifndef _POINT3_FACTORY_H_
#define _POINT3_FACTORY_H_

//forward declarations
class Point3;

#include "base/basevector4factory.h"

class Point3Factory : public BaseVector4Factory
{
	static Point3Factory * instance;

    public:

	static Point3Factory * GetInstance();

	Point3Factory();
    virtual ~Point3Factory();

    virtual BaseVector4 * CreatePermAttached(float * target);
    virtual void CreateArray(int count, BaseVector4 *** ppptr);
};

#endif