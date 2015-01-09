#ifndef _GTE_COLOR4_ARRAY_H_
#define _GTE_COLOR4_ARRAY_H_

//forward declarations
class Color4;

#include "base/basevector4array.h"

class Color4Array : public BaseVector4Array
{
    public:

	Color4Array();
    virtual ~Color4Array();

    void SetData(const float * data, bool includeW);
    Color4 * GetColor(int index);
    Color4 ** GetColors();
};

#endif
