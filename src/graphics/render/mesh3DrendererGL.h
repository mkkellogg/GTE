#ifndef _MESH3D_RENDERER_GL_H_
#define _MESH3D_RENDERER_GL_H_

// forward declarations
class EngineObjectManagerGL;
class VertexAttrBufferGL;
class Point3Array;
class Vector3Array;
class Color4Array;
class UV2Array;
class Mesh3D;
class Material;

#include "mesh3Drenderer.h"
#include "graphics/attributes.h"

class Mesh3DRendererGL : public Mesh3DRenderer
{
	friend class EngineObjectManagerGL;

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
};

#endif

