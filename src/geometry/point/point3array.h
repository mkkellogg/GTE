#ifndef _POINT3ARRAY_H_
#define _POINT3ARRAY_H_

//forward declarations
class Point3;

#include "base/basevector4array.h"

class Point3Array : public BaseVector4Array
{
    public:

	class Iterator : public BaseVector4Array::Iterator
	{
		friend class Point3Array;
		protected:
		Iterator(Point3Array& targetArray, Point3& targetPoint) : BaseVector4Array::Iterator((BaseVector4Array&)targetArray, (BaseVector4&)targetPoint) {}
	};

	Point3Array();
    virtual ~Point3Array();

    void SetData(const float * data, bool includeW);
    Point3 * GetPoint(int index);
    Point3 ** GetPoints();
    Iterator GetIterator(Point3& targetPoint);
};

#endif
