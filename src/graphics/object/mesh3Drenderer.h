#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

#include "graphics/vertexattrbuffer.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"

#include "geometry/point3array.h"
#include "geometry/vector3array.h"
#include "graphics/color4array.h"
#include "graphics/uv2array.h"

class Mesh3DRenderer
{
    VertexAttrBuffer * positionData;
    VertexAttrBuffer * normalData;
    VertexAttrBuffer * colorData;
    VertexAttrBuffer * uv1Data;
    VertexAttrBuffer * uv2Data;

    bool buffersOnGPU;

    bool InitBuffer(VertexAttrBuffer ** buffer, int attributeCount, int componentCount);
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBuffer ** buffer);
    void SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride);

    bool InitPositionData(int count);
    bool InitNormalData(int count);
    bool InitColorData(int count);
    bool InitUV1Data(int count);
    bool InitUV2Data(int count);

    void SetPositionData(Point3Array * points);
    void SetNormalData(Vector3Array * normals);
    void SetColorData(Color4Array * colors);
    void SetUV1Data(UV2Array * uvs);
    void SetUV2Data(UV2Array * uvs);

    public:

    Mesh3DRenderer();
    Mesh3DRenderer(bool buffersOnGPU);
    virtual ~Mesh3DRenderer();

   // void SetVertexData(const float * data, int componentCount, int count, AttributeType attributeType);
};

#endif

