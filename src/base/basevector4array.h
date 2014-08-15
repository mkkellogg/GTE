#ifndef _BASEVECTOR4ARRAY_H_
#define _BASEVECTOR4ARRAY_H_

#include "basevector4.h"
#include "basevector4factory.h"

class BaseVector4Array
{
	protected:

	int count;
	float * data;
	BaseVector4 ** objects;
	BaseVector4Factory * baseFactory;

	void Destroy();

    public:

	BaseVector4Array(int count, BaseVector4Factory * factory);
    virtual ~BaseVector4Array();

    bool Init();
};

#endif
