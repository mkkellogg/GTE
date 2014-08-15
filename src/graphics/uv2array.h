#ifndef _UV2_ARRAY_H_
#define _UV2_ARRAY_H_

#include "uv2.h"
#include "base/basevector2.h"
#include "base/basevector2array.h"

class UV2Array : public BaseVector2Array
{
    public:

	UV2Array(int count);
    virtual ~UV2Array();

    void SetData(const float * data);
    UV2 * GetCoordinate(int index);
    UV2 ** GetCoordinates();
};

#endif
