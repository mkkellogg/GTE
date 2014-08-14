#ifndef _POINT3ARRAY_H_
#define _POINT3ARRAY_H_

#include "point3.h"

class Point3Array
{
	float * data;
	Point3 ** objects;
	int count;

	bool Init(int count);
	void Destroy();

    public:

	Point3Array(int count);
    virtual ~Point3Array();

    void SetData(const float * data, bool includeW);
    Point3 * GetPoint(int index);
    Point3 ** GetPoints();
};

#endif
