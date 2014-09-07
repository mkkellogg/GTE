#ifndef _MESH3D_RENDERER_GL_H_
#define _MESH3D_RENDERER_GL_H_

// forward declarations
class GraphicsGL;
class Graphics;
class VertexAttrBufferGL;
class VertexAttrBuffer;
class Point3Array;
class Vector3Array;
class Color4Array;
class UV2Array;
class Mesh3D;
class Material;

#include "mesh3Drenderer.h"
#include "graphics/stdattributes.h"

class Mesh3DRendererGL : public Mesh3DRenderer
{
	friend class GraphicsGL;

	const static int MAX_ATTRIBUTE_BUFFERS = 64;

    VertexAttrBuffer * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];

	unsigned int storedVertexCount;
	StandardAttributeSet storedAttributes;
    bool buffersOnGPU;

    bool InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount,  int stride);
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBuffer ** buffer);
    void SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride);

    bool InitAttributeData(StandardAttribute attr, int componentCount,  int stride);

    void SetPositionData(Point3Array * points);
    void SetNormalData(Vector3Array * normals);
    void SetColorData(Color4Array * colors);
    void SetUV1Data(UV2Array * uvs);
    void SetUV2Data(UV2Array * uvs);

    protected:

    Mesh3DRendererGL(Graphics * graphics);
    Mesh3DRendererGL(bool buffersOnGPU, Graphics * graphics);
    virtual ~Mesh3DRendererGL();

    bool UseMaterial(Material * material);

    public:

    bool UseMesh(Mesh3D * newMesh);
    void CopyMeshData();
    void UpdateFromMesh();
    void Render();
};

#endif

