#ifndef _MESH3D_H_
#define _MESH3D_H_

#include "graphics/vertexattrbuffer.h"

enum class AttributeType
{
    Position,
    Normal,
    UV1,
    UV2
};

class Mesh3D
{
    VertexAttrBuffer * positionData;
    VertexAttrBuffer * normalData;
    VertexAttrBuffer * uv1Data;
    VertexAttrBuffer * uv2Data;

    bool InitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);
    bool CheckAndInitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);

    public:

    Mesh3D();
    virtual ~Mesh3D();

    void SetVertexData(const float * data, AttributeType attributeType);
};

#endif

