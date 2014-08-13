#ifndef _VECTOR3ARRAY_H_
#define _VECTOR3ARRAY_H_

#include "vector3.h"

class Vector3Array
{
	float * data;
	Vector3 ** objects;
	int count;

	bool Init(int count);
	void Destroy();

    public:

	Vector3Array(int count);
    virtual ~Vector3Array();

    void SetData(const float * data, bool includeW);
};

#endif
