#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/submesh3Dface.h"
#include "graphics/object/submesh3Dfaces.h"
#include "material.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/render/vertexattrbuffer.h"
#include "graphics/object/customfloatattributebuffer.h"
#include "mesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "geometry/point/point3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "object/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "attributetransformer.h"
#include "util/time.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"

namespace GTE
{
	/*
	* Constructor with pointer to an attribute transformer, buffersOnGPU = false by default
	*/
	SubMesh3DRenderer::SubMesh3DRenderer(AttributeTransformer * attributeTransformer) : SubMesh3DRenderer(false, attributeTransformer)
	{

	}

	/*
	 * Constructor with pointer to an attribute transformer, and parameter to choose CPU-side or GPU-side vertex attribute buffers
	 */
	SubMesh3DRenderer::SubMesh3DRenderer(Bool buffersOnGPU, AttributeTransformer * attributeTransformer)
	{
		this->lastUsedMaterialID = (UInt32)-1;
		this->containerRenderer = nullptr;
		this->targetSubMeshIndex = -1;

		memset(attributeBuffers, 0, sizeof(VertexAttrBuffer*) * MAX_ATTRIBUTE_BUFFERS);

		for (UInt32 i = 0; i < (UInt32)StandardAttribute::_Last; i++)
		{
			attributeBuffers[i] = nullptr;
		}

		storedVertexCount = 0;
		storedAttributes = StandardAttributes::CreateAttributeSet();

		this->buffersOnGPU = buffersOnGPU;
		this->attributeTransformer = attributeTransformer;

		doAttributeTransform = attributeTransformer == nullptr ? false : true;
		doPositionTransform = false;
		doNormalTransform = false;
		doTangentTransform = false;
		useBadGeometryShadowFix = false;

		updateCount = 0;
	}

	/*
	 * Clean-up
	 */
	SubMesh3DRenderer::~SubMesh3DRenderer()
	{
		Destroy();
	}

	/*
	 * Specify the index of this sub-renderer's target sub-mesh in the target
	 * mesh.
	 */
	void SubMesh3DRenderer::SetTargetSubMeshIndex(UInt32 index)
	{
		targetSubMeshIndex = index;
	}

	/*
	 * Set the pointer to the containing Mesh3DRender.
	 */
	void SubMesh3DRenderer::SetContainerRenderer(Mesh3DRenderer * renderer)
	{
		NONFATAL_ASSERT(renderer != nullptr, "SubMesh3DRenderer::SetContainerRenderer -> 'renderer' is null.", true);
		this->containerRenderer = renderer;
	}

	/*
	 * Deallocate and destroy all data & objects created by this sub-renderer.
	 */
	void SubMesh3DRenderer::Destroy()
	{
		DestroyBuffers();
		SAFE_DELETE(attributeTransformer);
	}

	/*
	 * Destroy all vertex attribute buffers that have been allocated.
	 */
	void SubMesh3DRenderer::DestroyBuffers()
	{
		for(UInt32 i =0; i < MAX_ATTRIBUTE_BUFFERS; i++)
		{
			if(attributeBuffers[i] != nullptr)
			{
				DestroyBuffer(&attributeBuffers[i]);
			}
		}
	}

	/*
	 * Destroy a single vertex attribute buffer and set its pointer to nullptr.
	 */
	void SubMesh3DRenderer::DestroyBuffer(VertexAttrBuffer ** buffer)
	{
		if ((*buffer) != nullptr)
		{
			Engine::Instance()->GetGraphicsSystem()->DestroyVertexAttributeBuffer(*buffer);
		}
		*buffer = nullptr;
	}

	/*
	 * Create and initialize an instance of VertexAttrBuffer.
	 */
	Bool SubMesh3DRenderer::InitBuffer(VertexAttrBuffer ** buffer, Int32 vertexCount, Int32 componentCount, Int32 stride, const Real * srcData)
	{
		NONFATAL_ASSERT_RTRN(buffer != nullptr, "SubMesh3DRenderer::InitBuffer -> Attempted to initialize vertex attribute buffer from null pointer.", false, true);

		// if the buffer has already been created, destroy it first.
		DestroyBuffer(buffer);

		// create the vertex attribute buffer
		*buffer = Engine::Instance()->GetGraphicsSystem()->CreateVertexAttributeBuffer();
		ASSERT(*buffer != nullptr, "SubMesh3DRenderer::InitBuffer -> Graphics::CreateVertexAttrBuffer() returned null.");
		// initialize the vertex attribute buffer
		(*buffer)->Init(vertexCount, componentCount, stride, buffersOnGPU, srcData);

		return true;
	}

	/*
	 * Create & initialize the vertex attribute buffer in [attributeBuffers] that corresponds to [attr].
	 */
	Bool SubMesh3DRenderer::InitAttributeData(UInt32 attr, Int32 length, Int32 componentCount, Int32 stride, const Real * srcData)
	{
		// if the buffer already exists, destroy it first
		DestroyBuffer(&attributeBuffers[attr]);
		// create and initialize buffer
		Bool initSuccess = InitBuffer(&attributeBuffers[attr], length, componentCount, stride, srcData);

		return initSuccess;
	}

	/*
	 * Get the number of times this renderer has been updated with its target sub-mesh.
	 */
	UInt32 SubMesh3DRenderer::GetUpdateCount() const
	{
		return updateCount;
	}

	/*
	 * Specify whether or not to use the fix for shadow volume artifacts that arise when mesh geometry is bad.
	 */
	void SubMesh3DRenderer::SetUseBadGeometryShadowFix(Bool useFix)
	{
		useBadGeometryShadowFix = useFix;
	}

	/*
	 * Build a shadow volume for this mesh. For point lights, the position of the light is in
	 * [lightPosDir], for directional lights the direction is also in [lightPosDir]. The 
	 * flag [directional] indicates if the light is directional or not.
	 *
	 * The algorithm utilized in this method was inspired by the method used in the GPU gems article:
	 *
	 * http://http.developer.nvidia.com/GPUGems3/gpugems3_ch11.html
	 *
	 * If [backFacesFrontCap] is true, in order to reduce Z-fighting artifacts with mesh polygons that face the light, this
	 * method will use a modified approach: Light-facing polygons are ignored completely. The front cap of the shadow volume
	 * is actually made up of the back-facing polygons (polygons facing away from the light). This still
	 * produces the correct effect with out the front-facing Z-fighting issues.
	 *
	 * The algorithm in this method also uses a couple techniques to get around the typical requirement that
	 * the mesh for which the shadow volume is being generated needs to be closed. By closed, we mean a mesh where
	 * each triangle has an adjacent triangles on every edge. When a triangle with an edge that doesn't have an
	 * adjacent face is encountered, the algorithm treats the problematic edge as if it were an edge separating
	 * front and back facing triangles, and generates side polygons for that edge.
	 *
	 * The above work-around doesn't fix all meshes and there is one more option for bad-meshes that still
	 * show artifacts (such as meshes with degenerate triangles). If the [useBadGeometryShadowFix] member variable is set,
	 * this algorithm generates a shadow volume for each back-facing triangle individually. This results in much more
	 * complex shadow volume geometry that incurs a significant performance penalty, but it will fix artifacts from really bad meshes.
	 */
	void SubMesh3DRenderer::BuildShadowVolume(const Vector3& lightPosDir, Bool directional, Bool backFacesFrontCap)
	{
		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::BuildShadowVolume -> Mesh is invalid.");

		if (ShouldUpdateFromMesh())this->UpdateFromMesh();

		// if this sub-renderer is utilizing an attribute transformer, we want to use the positions that result
		// from that transformation to build the shadow volume. otherwise we want to use the original positions
		// from the target sub-mesh.
		Point3Array * positions = mesh->GetPostions();
		ASSERT(positions, "SubMesh3DRenderer::BuildShadowVolume -> Mesh contains null positions array.");
		Point3Array& positionsSource = doPositionTransform == true ? transformedPositions : *positions;
		Real * positionsSrcPtr = const_cast<Real*>(positionsSource.GetDataPtr());

		// if this sub-renderer is utilizing an attribute transformer, we want to use the normals that result
		// from that transformation to build the shadow volume.
		Vector3Array * normals = mesh->GetFaceNormals();
		ASSERT(normals, "SubMesh3DRenderer::BuildShadowVolume -> MEsh contains null face normals array.");
		Vector3Array& normalsSource = doNormalTransform == true ? transformedFaceNormals : *normals;

		// dot product result threshold distinguishing front and back facing polygons.
		// the dot product is calculated between a triangle's normal, and the direction
		// vector to the light.
		Real backFaceThreshold = 0;

		// currentPositionVertexIndex = current number of process shadow volume vertices
		UInt32 currentPositionVertexIndex = 0;
		// use a raw pointer to the shadow volume position data because it's faster
		Real * svPositionBase = const_cast<Real*>(shadowVolumePositions.GetDataPtr());

		// structure that describes that relationship of the mesh faces, specifically which faces are adjacent
		SubMesh3DFaces& faces = mesh->GetFaces();
		UInt32 faceCount = faces.GetFaceCount();

		// temp working variables
		Real* faceVertex1 = nullptr;
		Real* faceVertex2 = nullptr;
		Real* faceVertex3 = nullptr;
		Vector3 faceToLightDir = lightPosDir;
		UInt32 faceVertexIndex = 0;
		Vector3 * faceNormal;
		SubMesh3DFace * face = nullptr;
		Point3 vertexAvg;

		// temp working variables related to adjacency processing
		Real* edgeVertex1 = nullptr;
		Real* edgeVertex2 = nullptr;
		Real* adjVertex1 = nullptr;
		Real* adjVertex2 = nullptr;
		Real* adjVertex3 = nullptr;
		Vector3 adjFaceToLightDir;
		UInt32 adjacentFaceVertexIndex = 0;
		Int32 adjacentFaceIndex = -1;
		Vector3 * adjacentFaceNormal = nullptr;
		SubMesh3DFace * adjacentFace = nullptr;

		// treat as directional light for now
		faceToLightDir.Normalize();
		faceToLightDir.Invert();
		adjFaceToLightDir = faceToLightDir;

		// loop through each face in [faces]
		for (UInt32 f = 0; f < faceCount; f++)
		{
			face = faces.GetFace(f);
			if (face == nullptr)continue;

			// face->FirstVertexIndex is the index of the face's first vertex in [positions] & [normals]
			faceVertexIndex = face->FirstVertexIndex;

			// get the face's normal
			faceNormal = normalsSource.GetVector(faceVertexIndex);

			// copy the three vertices of the face to [vertex1], [vertex2], and [vertex3]
			faceVertex1 = positionsSrcPtr + (faceVertexIndex << 2);
			faceVertex2 = positionsSrcPtr + (faceVertexIndex << 2) + 4;
			faceVertex3 = positionsSrcPtr + (faceVertexIndex << 2) + 8;

			// compute average of three vertices
			vertexAvg.x = (faceVertex1[0] + faceVertex2[0] + faceVertex3[0]) / 3;
			vertexAvg.y = (faceVertex1[1] + faceVertex2[1] + faceVertex3[1]) / 3;
			vertexAvg.z = (faceVertex1[2] + faceVertex2[2] + faceVertex3[2]) / 3;

			// if we have a point light, use the average of the vertices to calculate the direction to it
			if (!directional)
			{
				faceToLightDir.x = lightPosDir.x - vertexAvg.x;
				faceToLightDir.y = lightPosDir.y - vertexAvg.y;
				faceToLightDir.z = lightPosDir.z - vertexAvg.z;
				faceToLightDir.QuickNormalize();
			}

			// calculate dot product between face normal and direction to light
			Real faceToLightDot = Vector3::Dot(faceToLightDir, *faceNormal);

			if (faceToLightDot > backFaceThreshold) // we have a front facing triangle (facing the light)
			{
				// are we using back faces to build the volume front cap? if so we ignore this face
				// since it faces the light
				if (backFacesFrontCap)
				{
					continue;
				}

				// copy the three face vertices into the shadow volume position array [svPositionBase]
				BaseVector4_QuickCopy_IncDest(faceVertex1, svPositionBase);
				BaseVector4_QuickCopy_IncDest(faceVertex2, svPositionBase);
				BaseVector4_QuickCopy_IncDest(faceVertex3, svPositionBase);

				// copy the three face vertices into the shadow volume position array again, but zero out
				// the 4th component of the position vector. this allows the shadow volume shader to project
				// the points to infinity to create the back cap of the shadow volume.
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex3, svPositionBase);
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex2, svPositionBase);
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex1, svPositionBase);

				currentPositionVertexIndex += 6;
			}
			else if (faceToLightDot < backFaceThreshold)// we have a back facing triangle (facing away from the light)
			{
				// are we using front faces to build the volume front cap? if so we ignore this face
				// since it faces away from the light
				if (!backFacesFrontCap)
				{
					continue;
				}

				// copy the three face vertices into the shadow volume position array [svPositionBase]
				BaseVector4_QuickCopy_IncDest(faceVertex3, svPositionBase);
				BaseVector4_QuickCopy_IncDest(faceVertex2, svPositionBase);
				BaseVector4_QuickCopy_IncDest(faceVertex1, svPositionBase);

				// copy the three face vertices into the shadow volume position array again, but zero out
				// the 4th component of the position vector. this allows the shadow volume shader to project
				// the points to infinity to create the back cap of the shadow volume.
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex1, svPositionBase);
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex2, svPositionBase);
				BaseVector4_QuickCopy_ZeroW_IncDest(faceVertex3, svPositionBase);

				currentPositionVertexIndex += 6;
			}
			else continue;

			Int32 facesFound = 0;
			// loop through each edge of the face and examine the adjacent face
			for (UInt32 ai = 0; ai < 3; ai++)
			{
				adjacentFaceIndex = face->AdjacentFaceIndex1;
				edgeVertex1 = faceVertex1;
				edgeVertex2 = faceVertex2;

				if (ai == 1)
				{
					adjacentFaceIndex = face->AdjacentFaceIndex2;
					edgeVertex1 = faceVertex2;
					edgeVertex2 = faceVertex3;
				}
				else if (ai == 2)
				{
					adjacentFaceIndex = face->AdjacentFaceIndex3;
					edgeVertex1 = faceVertex3;
					edgeVertex2 = faceVertex1;
				}

				Real adjFaceToLightDot = 0;

				// adjacentFaceIndex >=0 means we found an adjacent face
				if (adjacentFaceIndex >= 0)
				{
					// adjacentFaceIndex will be the index in [faces] of the adjacent face
					facesFound++;
					adjacentFace = faces.GetFace(adjacentFaceIndex);
					if (adjacentFace != nullptr)
					{
						// adjacentFaceVertexIndex is the index of the adjacent face's first vertex in [positions] & [normals]
						adjacentFaceVertexIndex = adjacentFace->FirstVertexIndex;

						// get the adjacent face's normal
						adjacentFaceNormal = normalsSource.GetVector(adjacentFaceVertexIndex);

						// copy the three vertices of the adjacent face to [adjVertex1], [adjVertex2], and [adjVertex3]
						adjVertex1 = positionsSrcPtr + (adjacentFaceVertexIndex * 4);
						adjVertex2 = positionsSrcPtr + (adjacentFaceVertexIndex * 4) + 4;
						adjVertex3 = positionsSrcPtr + (adjacentFaceVertexIndex * 4) + 8;

						// compute average of three vertices
						vertexAvg.x = (adjVertex1[0] + adjVertex2[0] + adjVertex3[0]) / 3;
						vertexAvg.y = (adjVertex1[1] + adjVertex2[1] + adjVertex3[1]) / 3;
						vertexAvg.z = (adjVertex1[2] + adjVertex2[2] + adjVertex3[2]) / 3;

						// if we have a point light, use the average of the vertices to calculate the direction to it
						if (!directional)
						{
							adjFaceToLightDir.x = lightPosDir.x - vertexAvg.x;
							adjFaceToLightDir.y = lightPosDir.y - vertexAvg.y;
							adjFaceToLightDir.z = lightPosDir.z - vertexAvg.z;
							adjFaceToLightDir.QuickNormalize();
						}

						// calculate dot product between adjacent face normal and direction to light
						adjFaceToLightDot = Vector3::Dot(adjFaceToLightDir, *adjacentFaceNormal);
					}
				}
				else adjFaceToLightDot = 1;

				if (backFacesFrontCap) // are we using back faces to build the volume front cap?
				{
					// if the current face is back facing, and either:
					//    1. The adjacent face on the current edge is front facing (adjFaceToLightDot >= backFaceThreshold)
					//    2. There is no adjacent face (adjacentFaceIndex < 0)
					//    3. [useBadGeometryShadowFix] == true
					// then we create two side polygons to link the current face, which will be a front cap triangle,
					// to the back cap triangle, which is the current face projected to infinity, on the current edge
					if (adjFaceToLightDot > backFaceThreshold || adjacentFaceIndex < 0 || useBadGeometryShadowFix)
					{
						BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
						BaseVector4_QuickCopy_IncDest(edgeVertex2, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

						BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex1, svPositionBase);

						currentPositionVertexIndex += 6;
					}
				}
				else // we are using front faces to build the volume front cap?
				{
					// if the current face is front facing, and either:
					//    1. The adjacent face on the current edge is back facing (adjFaceToLightDot <= backFaceThreshold)
					//    2. There is no adjacent face (adjacentFaceIndex < 0)
					//    3. [useBadGeometryShadowFix] == true
					// then we create two side polygons to link the current face, which will be a front cap triangle,
					// to the back cap triangle, which is the current face projected to infinity, on the current edge
					if (adjFaceToLightDot < backFaceThreshold || adjacentFaceIndex < 0 || useBadGeometryShadowFix)
					{
						BaseVector4_QuickCopy_IncDest(edgeVertex2, svPositionBase);
						BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

						BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex1, svPositionBase);
						BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

						currentPositionVertexIndex += 6;
					}
				}
			}
		}
		shadowVolumePositions.SetCount(currentPositionVertexIndex);
	}

	const Point3Array * SubMesh3DRenderer::GetShadowVolumePositions()
	{
		return &shadowVolumePositions;
	}

	/*
	 * Set the vertex attribute buffer data for the shadow volume positions.
	 */
	void SubMesh3DRenderer::SetShadowVolumePositionData(const Point3Array * points)
	{
		attributeBuffers[(Int32)StandardAttribute::ShadowPosition]->SetData(points->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh vertex positions.
	 */
	void SubMesh3DRenderer::SetPositionData(Point3Array * points)
	{
		attributeBuffers[(Int32)StandardAttribute::Position]->SetData(points->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh vertex normals.
	 */
	void SubMesh3DRenderer::SetNormalData(Vector3Array * normals)
	{
		attributeBuffers[(Int32)StandardAttribute::Normal]->SetData(normals->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh face normals.
	 */
	void SubMesh3DRenderer::SetFaceNormalData(Vector3Array * faceNormals)
	{
		attributeBuffers[(Int32)StandardAttribute::FaceNormal]->SetData(faceNormals->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh vertex tangents.
	 */
	void SubMesh3DRenderer::SetTangentData(Vector3Array * tangents)
	{
		attributeBuffers[(Int32)StandardAttribute::Tangent]->SetData(tangents->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh vertex colors.
	 */
	void SubMesh3DRenderer::SetVertexColorData(Color4Array * colors)
	{
		attributeBuffers[(Int32)StandardAttribute::VertexColor]->SetData(colors->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh UV coordinates set 1.
	 */
	void SubMesh3DRenderer::SetUV1Data(UV2Array * uvs)
	{
		attributeBuffers[(Int32)StandardAttribute::UVTexture0]->SetData(uvs->GetDataPtr());
	}

	/*
	 * Set the vertex attribute buffer data for the mesh UV coordinates set 2.
	 */
	void SubMesh3DRenderer::SetUV2Data(UV2Array * uvs)
	{
		attributeBuffers[(Int32)StandardAttribute::UVTexture1]->SetData(uvs->GetDataPtr());
	}

	/*
	* Set the vertex attribute buffer data at [index] in [attributeBuffers] with [data].
	*/
	void SubMesh3DRenderer::SetAttributeData(UInt32 index, Real * data)
	{
		attributeBuffers[index]->SetData(data);
	}

	/*
	 * Update the vertex attribute buffers of this sub-renderer to reflect type & size of the attribute
	 * data in the target sub-mesh.
	 */
	Bool SubMesh3DRenderer::UpdateMeshAttributeBuffers()
	{
		// if vertex attribute buffers are already created, destroy them
		DestroyBuffers();
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Could not find matching sub mesh for sub renderer.");

		StandardAttributeSet meshAttributes = mesh->GetStandardAttributeSet();

		boundAttributeBuffers.clear();
		// loop through each standard attribute and create/initialize vertex attribute buffer for each
		for (UInt32 i = 0; i < (UInt32)StandardAttribute::_Last; i++)
		{
			StandardAttribute attr = (StandardAttribute)i;
			if (StandardAttributes::HasAttribute(meshAttributes, attr))
			{
				Int32 componentCount = 4;
				if (attr == StandardAttribute::UVTexture0 || attr == StandardAttribute::UVTexture1)componentCount = 2;

				Int32 stride = 0;

				Bool initSuccess = InitAttributeData((UInt32)attr, mesh->GetTotalVertexCount(), componentCount, stride, nullptr);
				ASSERT(initSuccess, "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Could not initialize attribute data.");

				VertexAttrBufferBinding binding(attributeBuffers[(UInt32)attr], AttributeDirectory::GetStandardVarID(attr));
				boundAttributeBuffers.push_back(binding);
			}
		}

		UInt32 firstCustomAttributeIndex = GetFirstCustomAttributeBufferIndex();
		for(UInt32 i = 0; i < mesh->GetCustomFloatAttributeBufferCount(); i++)
		{
			CustomFloatAttributeBuffer * attrBuffer = mesh->GetCustomFloatAttributeBufferByOrder(i);
			ASSERT(attrBuffer != nullptr, "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Null custom attribute found.");

			UInt32 attributeBufferIndex = firstCustomAttributeIndex + i;
			Bool initSuccess = InitAttributeData(attributeBufferIndex, attrBuffer->GetSize(), attrBuffer->GetComponentCount(), 0, attrBuffer->GetDataPtr());
			ASSERT(initSuccess, "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Could not initialize attribute data.");

			VertexAttrBufferBinding binding(attributeBuffers[attributeBufferIndex], attrBuffer->GetAttributeID());
			boundAttributeBuffers.push_back(binding);
		}

		// update the local vertex count
		storedVertexCount = mesh->GetTotalVertexCount();

		// TODO: current shadow volume data memory is allocated for all mesh renderers, regardless if they cast a
		// shadow or not. This could be quite wasteful, so implement a way to avoid this excess memory usage.
		Bool shadowVolumeInitSuccess = true;
		shadowVolumeInitSuccess = shadowVolumePositions.Init(storedVertexCount * 8);
		shadowVolumeInitSuccess &= InitAttributeData((UInt32)StandardAttribute::ShadowPosition, storedVertexCount * 8, 4, 0, nullptr);
		ASSERT(shadowVolumeInitSuccess, "SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Error occurred while initializing shadow volume array.");

		boundShadowVolumeAttributeBuffers.clear();
		VertexAttrBufferBinding shadowVolumePositionBinding(attributeBuffers[(Int32)StandardAttribute::ShadowPosition], AttributeDirectory::GetStandardVarID(StandardAttribute::ShadowPosition));
		boundShadowVolumeAttributeBuffers.push_back(shadowVolumePositionBinding);

		storedAttributes = meshAttributes;

		return true;
	}

	/*
	 * If the attribute transformer for this sub-renderer is active, then it needs to allocate the
	 * correct amount of space to store the transformed copies of those attributes. This method
	 * accomplishes that.
	 */
	Bool SubMesh3DRenderer::UpdateAttributeTransformerData()
	{
		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::UpdateAttributeTransformerData -> Could not find matching sub mesh for sub renderer.");

		StandardAttributeSet meshAttributes = mesh->GetStandardAttributeSet();

		doPositionTransform = false;
		doNormalTransform = false;

		if (doAttributeTransform)
		{
			StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

			if (StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
				StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
			{
				Point3Array * positions = mesh->GetPostions();
				UInt32 positionCount = positions->GetCount();
				if (positionCount != transformedPositions.GetCount())
				{
					Bool initSuccess = transformedPositions.Init(positionCount);
					ASSERT(initSuccess == true, "SubMesh3DRenderer::UpdateAttributeTransformerData -> Unable to init transformed positions array.");

				}

				doPositionTransform = true;
			}

			if (StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal) &&
				StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
			{
				Vector3Array * normals = mesh->GetVertexNormals();
				UInt32 normalCount = normals->GetCount();
				if (transformedVertexNormals.GetCount() != normalCount)
				{
					Bool initSuccess = transformedVertexNormals.Init(normalCount);
					if (initSuccess == true)initSuccess = initSuccess && transformedFaceNormals.Init(normalCount);
					ASSERT(initSuccess == true, "SubMesh3DRenderer::UpdateAttributeTransformerData -> Unable to init transformed normals array.");
				}

				doNormalTransform = true;
			}

			if (StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Tangent) &&
				StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Tangent))
			{
				Vector3Array * tangents = mesh->GetVertexTangents();
				UInt32 tangentCount = tangents->GetCount();
				if (transformedVertexTangents.GetCount() != tangentCount)
				{
					Bool initSuccess = transformedVertexTangents.Init(tangentCount);
					ASSERT(initSuccess == true, "SubMesh3DRenderer::UpdateAttributeTransformerData -> Unable to init transformed tangents array.");
				}

				doTangentTransform = true;
			}
		}

		return true;
	}

	/*
	* Should this renderer update its data from its target mesh?
	*/
	Bool SubMesh3DRenderer::ShouldUpdateFromMesh()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::ShouldUpdateFromMesh -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::ShouldUpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

		if (mesh->GetUpdateCount() != GetUpdateCount())return true;
		return false;
	}

	/*
	 * Copy the attribute data from the target sub-mesh into the local vertex attribute buffers.
	 */
	void SubMesh3DRenderer::CopyMeshData()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::CopyMeshData -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::CopyMeshData -> Could not find matching sub mesh for sub renderer.");

		StandardAttributeSet meshAttributes = mesh->GetStandardAttributeSet();

		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetVertexNormals());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::FaceNormal))SetFaceNormalData(mesh->GetFaceNormals());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Tangent))SetTangentData(mesh->GetVertexTangents());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetVertexColorData(mesh->GetColors());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVs0());
		if (StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVs1());

		UInt32 firstCustomAttributeIndex = GetFirstCustomAttributeBufferIndex();
		for(UInt32 i = 0; i < mesh->GetCustomFloatAttributeBufferCount(); i++)
		{
			CustomFloatAttributeBuffer * attrBuffer = mesh->GetCustomFloatAttributeBufferByOrder(i);
			ASSERT(attrBuffer != nullptr, "SubMesh3DRenderer::CopyMeshData -> Null custom attribute found.");
			SetAttributeData(i + firstCustomAttributeIndex, attrBuffer->GetDataPtr());
		}
	}

	/*
	 * Update the time this renderer was last updated with its target sub-mesh.
	 */
	void SubMesh3DRenderer::UpdateUpdateCount()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::UpdateTimeStamp -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::UpdateTimeStamp -> Could not find matching sub mesh for sub renderer.");

		// make sure the time stamp equals the target mesh's timestamp exactly,
		// not the current time
		updateCount = mesh->GetUpdateCount();
	}

	/*
	* Get the index in [attributeBuffers] where storage of custom attribute buffers begins.
	*/
	UInt32 SubMesh3DRenderer::GetFirstCustomAttributeBufferIndex()
	{
		return (UInt32)StandardAttribute::_Last;
	}

	/*
	 * Update this sub-renderer to be consistent with the current state of its target sub-mesh. Whenever the target sub-mesh
	 * is updated (e.g. vertex positions are modified, or UV coordinates are added), the vertex attribute buffers in this
	 * sub-renderer need to be updated to contain the same data. This method makes sure the size of the of the vertex attribute
	 * buffers match the size of the corresponding attribute arrays in the target sub-mesh, and copies over the data afterwards.
	 *
	 * Mesh3D has an Update() that can be called manually whenever the attributes of any sub-meshes are modified. That method
	 * will in turn trigger the Update() method of each sub-mesh, which will in-turn call UpdateFromMesh() on the corresponding
	 * sub-renderer.
	 */
	void SubMesh3DRenderer::UpdateFromMesh()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::UpdateFromMesh -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::UpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

		Bool updateSuccess = true;

		// if the vertex count of this sub-renderer does not match that of the target sub-mesh, call
		// the UpdateMeshAttributeBuffers() method to resize
		if (mesh->GetTotalVertexCount() != storedVertexCount || mesh->GetStandardAttributeSet() != storedAttributes)
		{
			updateSuccess = updateSuccess && UpdateMeshAttributeBuffers();
		}

		// update this sub-renderer's attribute transformer so that its storage space for transformed vertex attributes
		// is large enough for the target sub-mesh
		updateSuccess = updateSuccess && UpdateAttributeTransformerData();
		ASSERT(updateSuccess == true, "SubMesh3DRenderer::UpdateFromMesh -> Error occurred while updating mesh structure and data.");

		// copy over the data from the target sub-mesh
		CopyMeshData();

		UpdateUpdateCount();
	}

	/*
	 * Validate [material] to be used with this sub-renderer and its target sub-mesh. This means making sure that the attributes
	 * expected by the shader belonging to [material] match the attributes that are supplied by the target sub-mesh. It also
	 * means calling VerifySetVars() to ensure all uniforms & attributes expected by the shader have been set correctly.
	 */
	Bool SubMesh3DRenderer::ValidateMaterialForMesh(MaterialRef material)
	{
		// don't bother validating this material if it has already been validated
		if (material->GetObjectID() == lastUsedMaterialID)return true;

		lastUsedMaterialID = material->GetObjectID();

		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::UseMaterial -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::UseMaterial -> Could not find matching sub mesh for sub renderer.");

		StandardAttributeSet materialAttributes = material->GetStandardAttributes();
		StandardAttributeSet meshAttributes = mesh->GetStandardAttributeSet();

		// look for mismatched shader attributes and mesh attributes
		for (Int32 i = 0; i < (Int32)StandardAttribute::_Last; i++)
		{
			StandardAttribute attr = (StandardAttribute)i;

			if (StandardAttributes::HasAttribute(materialAttributes, attr))
			{
				if (!StandardAttributes::HasAttribute(meshAttributes, attr))
				{
					std::string msg = std::string("Shader was expecting attribute ") + StandardAttributes::GetAttributeName(attr) + std::string(" but mesh does not have it.");
					Debug::PrintWarning(msg);
				}
			}
		}

		return true;
	}

	/*
	 * Set the attribute transformer to be used by this sub-renderer.
	 */
	void SubMesh3DRenderer::SetAttributeTransformer(AttributeTransformer * attributeTransformer)
	{
		this->attributeTransformer = attributeTransformer;
		if (attributeTransformer == nullptr)
		{
			this->doAttributeTransform = false;
		}
		else
		{
			this->doAttributeTransform = true;
			UpdateAttributeTransformerData();
			CopyMeshData();
		}
	}

	/*
	 * Get the attribute transformer that is used
	 */
	AttributeTransformer * SubMesh3DRenderer::GetAttributeTransformer()
	{
		return attributeTransformer;
	}

	/*
	 * Return the [doAttributeTransform] flag.
	 */
	Bool SubMesh3DRenderer::DoesAttributeTransform() const
	{
		return doAttributeTransform;
	}

	/*
	 * Perform all processing & transformations that need to occur before the target sub-mesh is
	 * actually rendered. This includes invoking the attribute transformer, if one exists.
	 *
	 * [model] - The model matrix for the target sub-mesh. This matrix contains the local->world-space transformation.
	 * [modelInverse] - The inverse of [model].
	 */
	void SubMesh3DRenderer::PreRender(const Matrix4x4& model, const Matrix4x4& modelInverse)
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRenderer::PreRender -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::PreRender -> Could not find matching sub mesh for sub renderer.");

		if (doAttributeTransform)
		{
			// pass the local->world-space transformation matrix and its inverse to the attribute transformer
			attributeTransformer->SetModelMatrix(model, modelInverse);

			Point3Array * positions = mesh->GetPostions();
			Vector3Array * vertexNormals = mesh->GetVertexNormals();
			Vector3Array * faceNormals = mesh->GetFaceNormals();
			Vector3Array * vertexTangents = mesh->GetVertexTangents();

			ASSERT(positions != nullptr && vertexNormals != nullptr, "SubMesh3DRenderer::PreRender -> Mesh contains null positions or normals.");
			ASSERT(vertexNormals != nullptr, "SubMesh3DRenderer::PreRender -> Mesh contains null vertex normals.");
			ASSERT(faceNormals != nullptr, "SubMesh3DRenderer::PreRender -> Mesh contains null face normals.");
			ASSERT(vertexTangents != nullptr, "SubMesh3DRenderer::PreRender -> Mesh contains null vertex tangents.");

			// invoke the attribute transformer
			attributeTransformer->TransformAttributes(*positions, transformedPositions,
				*vertexNormals, transformedVertexNormals,
				*faceNormals, transformedFaceNormals,
				*vertexTangents, transformedVertexTangents,
				mesh->GetCenter(), transformedCenter,
				doPositionTransform, doNormalTransform, doTangentTransform);

			// update the positions vertex attribute buffer with transformed positions
			if (doPositionTransform)SetPositionData(&transformedPositions);

			// update the normals vertex attribute buffer with transformed normals
			if (doNormalTransform)SetNormalData(&transformedVertexNormals);

			// update the tangents vertex attribute buffer with transformed tangents
			if (doTangentTransform)SetTangentData(&transformedVertexTangents);
		}
	}

	/*
	 * Get the position of the center of the target sub-mesh after attribute transformation. If the
	 * attribute transformer is not being used, then this value will be the same as the target
	 * sub-mesh's existing center.
	 */
	const Point3* SubMesh3DRenderer::GetFinalCenter() const
	{
		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRenderer::Render -> Could not find matching sub mesh for sub renderer.");

		StandardAttributeSet meshAttributes = mesh->GetStandardAttributeSet();

		if (doAttributeTransform)
		{
			StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

			if (StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
				StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
			{
				return &transformedCenter;
			}
		}

		return &(mesh->GetCenter());
	}

	/*
	 * Render the target sub-mesh.
	 */
	void SubMesh3DRenderer::Render()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRendererGL::Render -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

		if (ShouldUpdateFromMesh())this->UpdateFromMesh();

		MaterialRef currentMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
		ASSERT(ValidateMaterialForMesh(currentMaterial), "SubMesh3DRendererGL::Render -> Invalid material for the current mesh.");

		Engine::Instance()->GetGraphicsSystem()->RenderTriangles(boundAttributeBuffers, mesh->GetTotalVertexCount(), true);
	}

	/*
	 * Render the target sub-mesh's shadow volume.
	 */
	void SubMesh3DRenderer::RenderShadowVolume()
	{
		ASSERT(containerRenderer != nullptr, "SubMesh3DRendererGL::RenderShadowVolume -> Container renderer is null.");

		SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
		ASSERT(mesh.IsValid(), "SubMesh3DRendererGL::RenderShadowVolume -> Could not find matching sub mesh for sub renderer.");

		RenderShadowVolume(&shadowVolumePositions);
	}

	/*
	 * Render the shadow volume stored in [positions].
	 */
	void SubMesh3DRenderer::RenderShadowVolume(const Point3Array * shadowVolumePositions)
	{
		ASSERT(shadowVolumePositions != nullptr, "SubMesh3DRendererGL::RenderShadowVolume -> 'shadowVolumePositions' is null.");

		if (shadowVolumePositions->GetCount() > 0)
		{
			// set the shadow volume vertex attribute buffer data
			SetShadowVolumePositionData(shadowVolumePositions);

			// render shadow volume
			Engine::Instance()->GetGraphicsSystem()->RenderTriangles(boundShadowVolumeAttributeBuffers, shadowVolumePositions->GetCount(), false);
		}
	}
}
