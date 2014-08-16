#ifndef _BASEVECTOR2ARRAY_H_
#define _BASEVECTOR2ARRAY_H_

#include "basevector2.h"
#include "basevector2factory.h"

class BaseVector2Array
{
	protected:

	int count;
	float * data;
	BaseVector2 ** objects;
	BaseVector2Factory * baseFactory;

	void Destroy();

    public:

	BaseVector2Array(int count, BaseVector2Factory * factory);
    virtual ~BaseVector2Array();

    const float * GetDataPtr() const;
    bool Init();
};

#endif
