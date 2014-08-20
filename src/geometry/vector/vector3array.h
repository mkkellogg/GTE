#ifndef _VECTOR3ARRAY_H_
#define _VECTOR3ARRAY_H_

#include "vector3.h"
#include "base/basevector4.h"
#include "base/basevector4array.h"

class Vector3Array : public BaseVector4Array
{
    public:

	Vector3Array();
    virtual ~Vector3Array();

    void SetData(const float * data, bool includeW);
    Vector3 * GetVector(int index);
    Vector3 ** GetVectors();
};
#endif
