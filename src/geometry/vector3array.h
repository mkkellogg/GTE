#ifndef _VECTOR3ARRAY_H_
#define _VECTOR3ARRAY_H_

class Vector3Array
{
    public:

	Vector3Array(int count);
    virtual ~Vector3Array();

    void SetData(const float * data);
};

#endif
