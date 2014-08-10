#ifndef _MESH3D_H_
#define _MESH3D_H_

#include "graphics/vertexattrbuffer.h"

enum class AttributeType
{
    POSITION,
    NORMAL,
    UV1,
    UV2
};

class Mesh3D
{
    VertexAttrBuffer * positionData;
    VertexAttrBuffer * normalData;
    VertexAttrBuffer * uv1Data;
    VertexAttrBuffer * uv2Data;

    public:

    Mesh3D();
    virtual ~Mesh3D();
};

#endif

