#ifndef _MESH3D_H_
#define _MESH3D_H_

#include "graphics/vertexattrbuffer.h"

enum class AttributeType
{
    Position,
    Normal,
    Color,
    UV1,
    UV2
};

class Mesh3D
{
    VertexAttrBuffer * positionData;
    VertexAttrBuffer * normalData;
    VertexAttrBuffer * colorData;
    VertexAttrBuffer * uv1Data;
    VertexAttrBuffer * uv2Data;

    bool InitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);
    bool CheckAndInitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);
    void DestroyBuffers();
    void SetVertexData(const float * data, int componentCount, int totalCount, int stride);

    public:

    Mesh3D();
    virtual ~Mesh3D();

    void SetVertexData(const float * data, int count, AttributeType attributeType);
};

#endif

