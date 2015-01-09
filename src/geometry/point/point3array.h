#ifndef _GTE_POINT3ARRAY_H_
#define _GTE_POINT3ARRAY_H_

//forward declarations
class Point3;

#include "base/basevector4array.h"

class Point3Array : public BaseVector4Array
{
    public:

	Point3Array();
    virtual ~Point3Array();

    void SetData(const float * data, bool includeW);
    Point3 * GetPoint(int index);
    const Point3 * GetPointConst(int index) const;
    Point3 ** GetPoints();
};

#endif
