#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "matrix.h"
#include "global/constants.h"

class Transform
{
    public:

    Transform();
    Transform(Matrix * m);
    ~Transform();

    static void BuildProjectionMatrix(Matrix * m,float fov, float ratio, float nearP, float farP);
    static void BuildLookAtMatrix(Matrix * m, float posX, float posY, float posZ,
            						float lookAtX, float lookAtY, float lookAtZ);
};

#endif
