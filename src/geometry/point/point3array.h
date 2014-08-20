#ifndef _POINT3ARRAY_H_
#define _POINT3ARRAY_H_

#include "point3.h"
#include "base/basevector4.h"
#include "base/basevector4array.h"

class Point3Array : public BaseVector4Array
{
    public:

	Point3Array();
    virtual ~Point3Array();

    void SetData(const float * data, bool includeW);
    Point3 * GetPoint(int index);
    Point3 ** GetPoints();
};

#endif
