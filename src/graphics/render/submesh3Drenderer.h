#ifndef _SUBMESH3D_RENDERER_H_
#define _SUBMESH3D_RENDERER_H_

// forward declarations
class GraphicsGL;
class Graphics;
class VertexAttrBufferGL;
class VertexAttrBuffer;
class Color4Array;
class UV2Array;
class SubMesh3D;
class Material;
class Matrix4x4;

#include "object/sceneobjectcomponent.h"
#include "object/enginetypes.h"
#include "submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "attributetransformer.h"
#include "geometry/vector/vector3array.h"
#include "geometry/point/point3array.h"
#include "geometry/point/point3.h"

class SubMesh3DRenderer : public EngineObject
{
	friend class Mesh3DRenderer;
	friend class Graphics;

	protected:

	int subIndex;
	MaterialRef activeMaterial;
	Graphics * graphics;
	Mesh3DRenderer * containerRenderer;

	void SetContainerRenderer(Mesh3DRenderer * renderer);
	void SetSubIndex(unsigned int index);

	const static int MAX_ATTRIBUTE_BUFFERS = 64;
    VertexAttrBuffer * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];

	unsigned int storedVertexCount;
	StandardAttributeSet storedAttributes;
    bool buffersOnGPU;

    bool doAttributeTransform;
    bool doPositionTransform;
    bool doNormalTransform;
    bool useBadGeometryShadowFix;

    AttributeTransformer * attributeTransformer;
    Point3Array transformedPositions;
    Vector3Array transformedNormals;
    Vector3Array transformedStraightNormals;
    Point3 transformedCenter;

    Point3Array shadowVolumeFront;
    Point3Array shadowVolumeBack;
    Point3Array shadowVolumeSides;

    bool InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount,  int stride);
    void Destroy();
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBuffer ** buffer);
    void SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride);

    bool InitAttributeData(StandardAttribute attr, int length, int componentCount,  int stride);

    void SetShadowVolumePositionData(Point3Array * points);
    void SetPositionData(Point3Array * points);
    void SetNormalData(Vector3Array * normals);
    void SetVertexColorData(Color4Array * colors);
    void SetUV1Data(UV2Array * uvs);
    void SetUV2Data(UV2Array * uvs);

    bool UseMaterial(MaterialRef material, bool forShadowVoume);
    bool UpdateMeshAttributeBuffers();
    bool UpdateAttributeTransformerData();

    SubMesh3DRenderer(Graphics * graphics, AttributeTransformer * attributeTransformer);
    SubMesh3DRenderer(bool buffersOnGPU, Graphics * graphics, AttributeTransformer * attributeTransformer);

    void CopyMeshData();

	protected:

    virtual ~SubMesh3DRenderer();

    public:

    void SetUseBadGeometryShadowFix(bool useFix);

    void BuildShadowVolume(Vector3& lightPosDir, bool directional);
    void UpdateFromMesh();

    void SetAttributeTransformer(AttributeTransformer * attributeTransformer);
    AttributeTransformer * GetAttributeTransformer();
    bool DoesAttributeTransform();

    const Point3* GetFinalCenter();

    void PreRender(const Matrix4x4& modelView, const Matrix4x4& modelViewInverse);

    virtual void Render() = 0;
    virtual void RenderShadowVolume() = 0;
};

#endif

