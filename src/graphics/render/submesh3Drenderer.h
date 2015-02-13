/*
 * class: SubMesh3DRenderer
 *
 * author: Mark Kellogg
 *
 * SubMesh3DRenderer is responsible for rendering a single SubMesh3D object. A
 * SubMesh3DRenderer instance will be attached to an instance of Mesh3DRenderer, which
 * will contain other instances of SubMesh3DRenderer. Each instance of SubMesh3DRenderer is
 * responsible for rendering exactly one of the sub-meshes in the containing Mesh3DRenderer
 * instance's target mesh, which is a Mesh3D object.
 *
 * SubMesh3DRenderer has the concept of an attribute transformer built in, that is
 * an instance of AttributeTransformer or a deriving class can be supplied to it and it
 * will use that instance to transform the various attributes of its target mesh before it
 * does its rendering. An example would be SkinnedMesh3DRenderer, which supplies a special
 * kind of AttributeTransformer that performs vertex skinning.
 *
 * Certain functionality for performing rendering, such as making the call into the chosen
 * API to actually draw the target sub-mesh's triangles, will vary from platform to
 * platform (e.g. from OpenGL to DirectX). SubMesh3DRenderer is designed so that this
 * platform specific code should be in a deriving class (such as SubMesh3DRendererGL for OpenGL).
 */


#ifndef _GTE_SUBMESH3D_RENDERER_H_
#define _GTE_SUBMESH3D_RENDERER_H_

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

#include <vector>
#include "object/sceneobjectcomponent.h"
#include "object/enginetypes.h"
#include "submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "graphics/render/material.h"
#include "attributetransformer.h"
#include "geometry/vector/vector3array.h"
#include "geometry/point/point3array.h"
#include "geometry/point/point3.h"

class SubMesh3DRenderer : public EngineObject
{
	// need to set Mesh3DRenderer as a friend so it can call the
	// protected method SetContainerRenderer()
	friend class Mesh3DRenderer;
	friend class EngineObjectManager;

	protected:

	// index of this sub-renderer in containing Mesh3DRenderer instance's list of sub-renderers
	int targetSubMeshIndex;
	// a reference to the last valid material used to for rendering
	MaterialRef lastUsedMaterial;
	// pointer to the Mesh3DRenderer instance that contains this instance of SubMesh3DRenderer
	Mesh3DRenderer * containerRenderer;

	// attribute buffers store vertex attribute data such as positions, normals, UV coordinates, etc.
	// they are copies of the attribute data stored in this renderer's target sub-mesh, but are stored in
	// a manner that is more suitable for delivery to the GPU
	const static int MAX_ATTRIBUTE_BUFFERS = 64;
    VertexAttrBuffer * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];
    std::vector<VertexAttrBufferBinding> boundAttributeBuffers;
    std::vector<VertexAttrBufferBinding> boundShadowVolumeAttributeBuffers;

    // number of vertices for which vertex attributes in [attributeBuffers] are stored
	unsigned int storedVertexCount;
	// mask that describes the different types of attributes stored in [storedAttributes]
	StandardAttributeSet storedAttributes;
	// are the vertex attributes stored in GPU-based buffers?
    bool buffersOnGPU;

    // doAttributeTransform == true means this sub-renderer's attribute transformer should be used to
    // transform vertex attributes prior to rendering
    bool doAttributeTransform;
    // should the attribute transform transform vertex positions?
    bool doPositionTransform;
    // should the attribute transform transform vertex normals?
    bool doNormalTransform;

    // pointer to this sub-renderer's attribute transformer
    AttributeTransformer * attributeTransformer;
    // if vertex positions are transformed, the transformed positions are stored here
    Point3Array transformedPositions;
    // if vertex positions are transformed, the transformed center position is stored here
    Point3 transformedCenter;
    // if normals are transformed, the transformed vertex normals are stored here
    Vector3Array transformedVertexNormals;
    // if normals are transformed, the transformed face normals are stored here
    Vector3Array transformedFaceNormals;

    // shadow volume vertex positions, when the shadow volume for this sub-renderer's target sub-mesh is created,
    // its geometry is stored here
    Point3Array shadowVolumePositions;
    // do some extra processing that will fix shadow volume artifacts that arise when mesh geometry is bad,
    // this incurs a performance penalty because it results in a shadow volume with many more triangles
    bool useBadGeometryShadowFix;
    // use an alternate shadow volume technique where the front caps are made up
    // up the back-facing triangles
    bool doBackSetShadowVolume;

    void SetContainerRenderer(Mesh3DRenderer * renderer);
    void SetTargetSubMeshIndex(unsigned int index);

    bool InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount,  int stride);
    void Destroy();
    void DestroyBuffers();
    void DestroyBuffer(VertexAttrBuffer ** buffer);
    bool InitAttributeData(StandardAttribute attr, int length, int componentCount,  int stride);

    void SetShadowVolumePositionData(Point3Array * points);
    void SetPositionData(Point3Array * points);
    void SetNormalData(Vector3Array * normals);
    void SetVertexColorData(Color4Array * colors);
    void SetUV1Data(UV2Array * uvs);
    void SetUV2Data(UV2Array * uvs);

    bool ValidateMaterialForMesh(MaterialRef material);
    bool UpdateMeshAttributeBuffers();
    bool UpdateAttributeTransformerData();

    SubMesh3DRenderer(AttributeTransformer * attributeTransformer);
    SubMesh3DRenderer(bool buffersOnGPU, AttributeTransformer * attributeTransformer);
    virtual ~SubMesh3DRenderer();

    void CopyMeshData();

    public:

    void SetUseBadGeometryShadowFix(bool useFix);

    void BuildShadowVolume(Vector3& lightPosDir, bool directional, bool backFacesFrontCap);
    void UpdateFromMesh();

    void SetAttributeTransformer(AttributeTransformer * attributeTransformer);
    AttributeTransformer * GetAttributeTransformer();
    bool DoesAttributeTransform();

    const Point3* GetFinalCenter();

    void PreRender(const Matrix4x4& modelView, const Matrix4x4& modelViewInverse);

    void Render();
    void RenderShadowVolume();

    void SetUseBackSetShadowVolume(bool use);
    bool GetUseBackSetShadowVolume();
};

#endif

