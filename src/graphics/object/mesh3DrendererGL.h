#ifndef _MESH3D_RENDERER_GL_H_
#define _MESH3D_RENDERER_GL_H_

class Mesh3DRendererGL;

#include "mesh3D.h"
#include "mesh3Drenderer.h"

#include "graphics/vertexattrbufferGL.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"


class Mesh3DRendererGL : public Mesh3DRenderer
{
	friend class GraphicsGL;

	const static int MAX_ATTRIBUTE_BUFFERS = 64;

    VertexAttrBufferGL * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];

    bool buffersOnGPU;

    bool InitBuffer(VertexAttrBufferGL ** buffer, int vertexCount, int componentCount,  int stride);
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBufferGL ** buffer);
    void SetVertexData(VertexAttrBufferGL * buffer, const float * data, int componentCount, int totalCount, int stride);

    bool InitAttributeData(Attribute attr, int componentCount,  int stride);

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

    bool UseMesh(Mesh3D * newMesh);
    bool UseMaterial(Material * material);
    void Render();

   // void SetVertexData(const float * data, int componentCount, int count, AttributeType attributeType);
};

#endif

