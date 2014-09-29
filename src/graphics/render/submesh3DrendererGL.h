#ifndef _SUBMESH3D_RENDERER_GL_H_
#define _SUBMESH3D_RENDERER_GL_H_

// forward declarations
class GraphicsGL;
class Graphics;
class VertexAttrBufferGL;
class VertexAttrBuffer;
class Color4Array;
class UV2Array;
class SubMesh3D;
class Material;

#include "submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "attributetransformer.h"
#include "geometry/vector/vector3array.h"
#include "geometry/point/point3array.h"

class SubMesh3DRendererGL : public SubMesh3DRenderer
{
	friend class GraphicsGL;

	const static int MAX_ATTRIBUTE_BUFFERS = 64;

    VertexAttrBuffer * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];

	unsigned int storedVertexCount;
	StandardAttributeSet storedAttributes;
    bool buffersOnGPU;
    AttributeTransformer * attributeTransformer;
    bool doAttributeTransform;
    Point3Array positionsCopy, transformedPositions;
    Vector3Array normalsCopy, transformedNormals;

    bool InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount,  int stride);
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBuffer ** buffer);
    void SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride);

    bool InitAttributeData(StandardAttribute attr, int componentCount,  int stride);

    void SetPositionData(Point3Array * points);
    void SetNormalData(Vector3Array * normals);
    void SetVertexColorData(Color4Array * colors);
    void SetUV1Data(UV2Array * uvs);
    void SetUV2Data(UV2Array * uvs);

    protected:

    SubMesh3DRendererGL(Graphics * graphics, AttributeTransformer * attributeTransformer);
    SubMesh3DRendererGL(bool buffersOnGPU, Graphics * graphics, AttributeTransformer * attributeTransformer);
    virtual ~SubMesh3DRendererGL();

    bool UseMaterial(Material * material);

    public:

    bool UpdateMeshData();
    void CopyMeshData();
    void UpdateFromMesh();
    void Render();
};

#endif

