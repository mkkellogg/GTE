#ifndef _MESH3D_H_
#define _MESH3D_H_

#include "graphics/vertexattrbuffer.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"

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

    bool buffersOnGPU;

    bool InitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);
    bool CheckAndInitializeVertexAttrBuffer(VertexAttrBuffer ** buffer);
    void DestroyBuffers();
    void SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride);

    public:

    Mesh3D();
    Mesh3D(bool buffersOnGPU);
    virtual ~Mesh3D();

    void SetPositionData(Point3 ** points);
    void SetNormalData(Vector3 ** normals);
    void SetColorData(Color4 ** colors);
    void SetUV1Data(UV2 ** uvs);
    void SetUV2Data(UV2 ** uvs);
   // void SetVertexData(const float * data, int componentCount, int count, AttributeType attributeType);
};

#endif

