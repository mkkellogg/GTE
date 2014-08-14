#ifndef _COLOR4_ARRAY_H_
#define _COLOR4_ARRAY_H_

#include "color4.h"
#include "base/basevector4.h"
#include "base/basevector4array.h"

class Color4Array : public BaseVector4Array
{
    public:

	Color4Array(int count);
    virtual ~Color4Array();

    void SetData(const float * data, bool includeW);
    Color4 * GetColor(int index);
    Color4 ** GetColors();
};

#endif
