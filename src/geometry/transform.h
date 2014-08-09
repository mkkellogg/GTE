#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "matrix.h"

class Transform
{
    public:

    Transform();
    Transform(Matrix * m);
    ~Transform();
};

#endif
