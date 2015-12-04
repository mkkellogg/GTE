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

#include <vector>

#include "engine.h"
#include "scene/sceneobjectcomponent.h"
#include "graphics/stdattributes.h"
#include "graphics/render/material.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "attributetransformer.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3.h"

namespace GTE
{
	// forward declarations
	class GraphicsGL;
	class Graphics;
	class VertexAttrBufferGL;
	class VertexAttrBuffer;
	class SubMesh3D;
	class Material;
	class Matrix4x4;

	class SubMesh3DRenderer : public EngineObject
	{
		// need to set Mesh3DRenderer as a friend so it can call the
		// protected method SetContainerRenderer()
		friend class Mesh3DRenderer;
		// necessary since this derives from EngineObject and needs to be managed by EngineObjectManager
		friend class EngineObjectManager;
		// needed for special access during rendering
		friend class ForwardRenderManager;

		// index of this sub-renderer in containing Mesh3DRenderer instance's list of sub-renderers
		Int32 targetSubMeshIndex;
		// a reference to the last valid material used to for rendering
		UInt32 lastUsedMaterialID;
		// pointer to the Mesh3DRenderer instance that contains this instance of SubMesh3DRenderer
		Mesh3DRenderer * containerRenderer;

		// attribute buffers store vertex attribute data such as positions, normals, UV coordinates, etc.
		// they are copies of the attribute data stored in this renderer's target sub-mesh, but are stored in
		// a manner that is more suitable for delivery to the GPU
		const static Int32 MAX_ATTRIBUTE_BUFFERS = 64;
		VertexAttrBuffer * attributeBuffers[MAX_ATTRIBUTE_BUFFERS];
		std::vector<VertexAttrBufferBinding> boundAttributeBuffers;
		std::vector<VertexAttrBufferBinding> boundShadowVolumeAttributeBuffers;

		// number of vertices for which storage vertex attributes in [attributeBuffers] is allocated
		UInt32 totalVertexCount;
		// mask that describes the different types of attributes stored in [storedAttributes]
		StandardAttributeSet storedAttributes;
		// are the vertex attributes stored in GPU-based buffers?
		Bool buffersOnGPU;

		// number of times this renderer has been updated from its mesh
		UInt32 updateCount;

		// doAttributeTransform == true means this sub-renderer's attribute transformer should be used to
		// transform vertex attributes prior to rendering
		Bool doAttributeTransform;
		// should the attribute transform transform vertex positions?
		Bool doPositionTransform;
		// should the attribute transform vertex normals?
		Bool doNormalTransform;
		// should the attribute transform vertex tangents?
		Bool doTangentTransform;

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
		// if tangents are transformed, the transformed vertex tangents are stored here
		Vector3Array transformedVertexTangents;

		// shadow volume vertex positions, when the shadow volume for this sub-renderer's target sub-mesh is created,
		// its geometry is stored here
		Point3Array shadowVolumePositions;
		// do some extra processing that will fix shadow volume artifacts that arise when mesh geometry is bad,
		// this incurs a performance penalty because it results in a shadow volume with many more triangles
		Bool useBadGeometryShadowFix;

		void SetContainerRenderer(Mesh3DRenderer * renderer);
		void SetTargetSubMeshIndex(UInt32 index);

		Bool InitBuffer(VertexAttrBuffer ** buffer, Int32 vertexCount, Int32 componentCount, Int32 stride, const Real * srcData);
		void Destroy();
		void DestroyBuffers();
		void DestroyBuffer(VertexAttrBuffer ** buffer);
		Bool InitAttributeData(UInt32 attr, Int32 length, Int32 componentCount, Int32 stride, const Real * srcData);

		const Point3Array * GetShadowVolumePositions();
		void SetShadowVolumePositionData(const Point3Array * points);
		void SetPositionData(Point3Array * points);
		void SetNormalData(Vector3Array * normals);
		void SetFaceNormalData(Vector3Array * normals);
		void SetTangentData(Vector3Array * tangents);
		void SetVertexColorData(Color4Array * colors);
		void SetUV1Data(UV2Array * uvs);
		void SetUV2Data(UV2Array * uvs);
		void SetAttributeData(UInt32 index, const Real * data);

		Bool ValidateMaterialForMesh(MaterialRef material);
		Bool UpdateMeshAttributeBuffers();
		Bool UpdateAttributeTransformerData();

		SubMesh3DRenderer(AttributeTransformer * attributeTransformer);
		SubMesh3DRenderer(Bool buffersOnGPU, AttributeTransformer * attributeTransformer);
		virtual ~SubMesh3DRenderer();

		Bool ShouldUpdateFromMesh();
		void CopyMeshData();
		void UpdateUpdateCount();

		UInt32 GetFirstCustomAttributeBufferIndex();

	public:

		UInt32 GetUpdateCount() const;

		void SetUseBadGeometryShadowFix(Bool useFix);

		void BuildShadowVolume(const Vector3& lightPosDir, Bool directional, Bool backFacesFrontCap);
		void UpdateFromMesh();

		void SetAttributeTransformer(AttributeTransformer * attributeTransformer);
		AttributeTransformer * GetAttributeTransformer();
		Bool DoesAttributeTransform() const;

		const Point3* GetFinalCenter() const;

		void PreRender(const Matrix4x4& modelView, const Matrix4x4& modelViewInverse);

		void Render();
		void RenderShadowVolume();
		void RenderShadowVolume(const Point3Array * shadowVolumePositions);
	};
}

#endif

