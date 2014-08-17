#ifndef _MESH3D_RENDERER_GL_H_
#define _MESH3D_RENDERER_GL_H_

#include "mesh3D.h"
#include "mesh3Drenderer.h"

#include "graphics/vertexattrbufferGL.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"

#include "geometry/point3array.h"
#include "geometry/vector3array.h"
#include "graphics/color4array.h"
#include "graphics/uv2array.h"

class Mesh3DRendererGL : public Mesh3DRenderer
{
	friend class GraphicsGL;

	Mesh3D * mesh;

    VertexAttrBufferGL * positionData;
    VertexAttrBufferGL * normalData;
    VertexAttrBufferGL * colorData;
    VertexAttrBufferGL * uv1Data;
    VertexAttrBufferGL * uv2Data;

    bool buffersOnGPU;

    bool InitBuffer(VertexAttrBufferGL ** buffer, int attributeCount, int componentCount);
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBufferGL ** buffer);
    void SetVertexData(VertexAttrBufferGL * buffer, const float * data, int componentCount, int totalCount, int stride);

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

    protected:

    Mesh3DRendererGL();
    Mesh3DRendererGL(bool buffersOnGPU);
    virtual ~Mesh3DRendererGL();

    public:


    void Render();
    bool UseMesh(Mesh3D * newMesh);


   // void SetVertexData(const float * data, int componentCount, int count, AttributeType attributeType);
};

#endif

