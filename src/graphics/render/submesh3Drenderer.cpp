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
#include "global/global.h"
#include "debug/gtedebug.h"

/*
 * Constructor with pointer to an attribute transformer, buffersOnGPU = false by default
 */
SubMesh3DRenderer::SubMesh3DRenderer(AttributeTransformer * attributeTransformer) : SubMesh3DRenderer(false, attributeTransformer)
{

}

/*
 * Constructor with pointer to an attribute transformer, and parameter to choose CPU-side or GPU-side vertex attribute buffers
 */
SubMesh3DRenderer::SubMesh3DRenderer(bool buffersOnGPU, AttributeTransformer * attributeTransformer)
{
	this->containerRenderer = NULL;
	this->targetSubMeshIndex = -1;

	memset(attributeBuffers,0,sizeof(VertexAttrBuffer*) * MAX_ATTRIBUTE_BUFFERS);

	for(unsigned int i =0; i < (unsigned int)StandardAttribute::_Last; i++)
	{
		attributeBuffers[i] = NULL;
	}

	storedVertexCount = 0;
	storedAttributes = StandardAttributes::CreateAttributeSet();

	this->buffersOnGPU = buffersOnGPU;
	this->attributeTransformer = attributeTransformer;

	doAttributeTransform = attributeTransformer == NULL ? false : true;
	doPositionTransform = false;
	doNormalTransform = false;
	useBadGeometryShadowFix = false;
	doBackSetShadowVolume = true;
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
void SubMesh3DRenderer::SetTargetSubMeshIndex(unsigned int index)
{
	targetSubMeshIndex = index;
}

/*
 * Set the pointer to the containing Mesh3DRender.
 */
void SubMesh3DRenderer::SetContainerRenderer(Mesh3DRenderer * renderer)
{
	ASSERT_RTRN(renderer != NULL, "SubMesh3DRenderer::SetContainerRenderer -> renderer is NULL");
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
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Position]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Normal]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::VertexColor]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture0]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture1]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::ShadowPosition]);
}

/*
 * Destroy a single vertex attribute buffer and set its pointer to NULL.
 */
void SubMesh3DRenderer::DestroyBuffer(VertexAttrBuffer ** buffer)
{
	if((*buffer) != NULL)
	{
		Engine::Instance()->GetGraphicsEngine()->DestroyVertexAttributeBuffer(*buffer);
	}
	*buffer = NULL;
}

/*
 * Create and initialize an instance of VertexAttrBuffer.
 */
bool SubMesh3DRenderer::InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount, int stride)
{
	ASSERT(buffer != NULL,"SubMesh3DRenderer::InitBuffer -> Attempted to initialize vertex attribute buffer from null pointer.", false);

	// if the buffer has already been created, destroy it first.
	DestroyBuffer(buffer);

	// create the vertex attribute buffer
	*buffer = Engine::Instance()->GetGraphicsEngine()->CreateVertexAttributeBuffer();
	ASSERT(*buffer != NULL,"SubMesh3DRenderer::InitBuffer -> Graphics::CreateVertexAttrBuffer() returned NULL.", false);
	// initialize the vertex attribute buffer
	(*buffer)->Init(vertexCount, componentCount, stride, buffersOnGPU, NULL);

	return true;
}

/*
 * Create & initialize the vertex attribute buffer in [attributeBuffers] that corresponds to [attr].
 */
bool SubMesh3DRenderer::InitAttributeData(StandardAttribute attr, int length, int componentCount,  int stride)
{
	// if the buffer already exists, destroy it first
	DestroyBuffer(&attributeBuffers[(int)attr]);
	// create and initialize buffer
	bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], length, componentCount, stride);

	return initSuccess;
}

/*
 * Specify whether or not to use the fix for shadow volume artifacts that arise when mesh geometry is bad.
 */
void SubMesh3DRenderer::SetUseBadGeometryShadowFix(bool useFix)
{
	useBadGeometryShadowFix = useFix;
}

/*
 * Build a shadow volume for this mesh. For point lights, the position of the light is in
 * [lightPosDir], for directional lights the direction is also in [lightPosDir]. The boolean
 * flag [directional] indicates if the light is directional or not.
 *
 * The algorithm utilized in this method was inspired by the method used in the GPU gems article:
 *
 * http://http.developer.nvidia.com/GPUGems3/gpugems3_ch11.html
 *
 * However, in order to reduce Z-fighting artifacts with mesh polygons that face the light, this
 * method uses a modified approach: Light-facing polygons are ignored completely. The front cap of the shadow volume
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
void SubMesh3DRenderer::BuildShadowVolume(Vector3& lightPosDir, bool directional, bool backFacesFrontCap)
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(), "SubMesh3DRenderer::BuildShadowVolume -> mesh is invalid.");

	// if this sub-renderer is utilizing an attribute transformer, we want to use the positions that result
	// from that transformation to build the shadow volume. otherwise we want to use the original positions
	// from the target sub-mesh.
	Point3Array * positions = mesh->GetPostions();
	ASSERT_RTRN(positions, "SubMesh3DRenderer::BuildShadowVolume -> mesh contains NULL positions array.");
	Point3Array& positionsSource = doPositionTransform == true ? transformedPositions : *positions;
	float * positionsSrcPtr = const_cast<float*>(positionsSource.GetDataPtr());

	// if this sub-renderer is utilizing an attribute transformer, we want to use the normals that result
	// from that transformation to build the shadow volume.
	Vector3Array * normals = mesh->GetFaceNormals();
	ASSERT_RTRN(normals, "SubMesh3DRenderer::BuildShadowVolume -> mesh contains NULL face normals array.");
	Vector3Array& normalsSource = doNormalTransform == true ? transformedFaceNormals : *normals;

	// dot product result threshold distinguishing front and back facing polygons.
	// the dot product is calculated between a triangle's normal, and the direction
	// vector to the light.
	float backFaceThreshold = 0;

	// currentPositionVertexIndex = current number of process shadow volume vertices
	unsigned int currentPositionVertexIndex = 0;
	// use a raw pointer to the shadow volume position data because it's faster
	float * svPositionBase =  const_cast<float*>(shadowVolumePositions.GetDataPtr());

	// structure that describes that relationship of the mesh faces, specifically which faces are adjacent
	SubMesh3DFaces& faces = mesh->GetFaces();
	unsigned int faceCount = faces.GetFaceCount();

	// temp working variables
	float* faceVertex1 = NULL;
	float* faceVertex2 = NULL;
	float* faceVertex3 = NULL;
	Vector3 faceToLightDir = lightPosDir;
	unsigned int faceVertexIndex = 0;
	Vector3 * faceNormal;
	SubMesh3DFace * face = NULL;
	Point3 vertexAvg;

	// temp working variables related to adjacency processing
	float * edgeVertex1 = NULL;
	float * edgeVertex2 = NULL;
	float* adjVertex1 = NULL;
	float* adjVertex2 = NULL;
	float* adjVertex3 = NULL;
	Vector3 adjFaceToLightDir;
	unsigned int adjacentFaceVertexIndex = 0;
	int adjacentFaceIndex = -1;
	Vector3 * adjacentFaceNormal = NULL;
	SubMesh3DFace * adjacentFace = NULL;

	// treat as directional light for now
	faceToLightDir.Normalize();
	faceToLightDir.Invert();
	adjFaceToLightDir = faceToLightDir;

	// loop through each face in [faces]
	for(unsigned int f = 0; f < faceCount; f++)
	{
		face = faces.GetFace(f);
		if(face == NULL)continue;

		// face->FirstVertexIndex is the index of the face's first vertex in [positions] & [normals]
		faceVertexIndex = face->FirstVertexIndex;

		// get the face's normal
		faceNormal = normalsSource.GetVector(faceVertexIndex);

		// copy the three vertices of the face to [vertex1], [vertex2], and [vertex3]
		faceVertex1 = positionsSrcPtr+(faceVertexIndex << 2);
		faceVertex2 = positionsSrcPtr+(faceVertexIndex << 2)+4;
		faceVertex3 = positionsSrcPtr+(faceVertexIndex << 2)+8;

		// compute average of three vertices
		vertexAvg.x = (faceVertex1[0] + faceVertex2[0] + faceVertex3[0]) / 3;
		vertexAvg.y = (faceVertex1[1] + faceVertex2[1] + faceVertex3[1]) / 3;
		vertexAvg.z = (faceVertex1[2] + faceVertex2[2] + faceVertex3[2]) / 3;

		// if we have a point light, use the average of the vertices to calculate the direction to it
		if(!directional)
		{
			faceToLightDir.x = lightPosDir.x - vertexAvg.x;
			faceToLightDir.y = lightPosDir.y - vertexAvg.y;
			faceToLightDir.z = lightPosDir.z - vertexAvg.z;
			faceToLightDir.Normalize();
		}

		// calculate dot product between face normal and direction to light
		float faceToLightDot = Vector3::Dot(faceToLightDir, *faceNormal);

		if(faceToLightDot >= backFaceThreshold) // we have a front facing triangle (facing the light)
		{
			// are we using back faces to build the volume front cap? if so we ignore this face
			// since it faces the light
			if(backFacesFrontCap)
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
		else // we have a back facing triangle (facing away from the light)
		{
			// are we using front faces to build the volume front cap? if so we ignore this face
			// since it faces away from the light
			if(!backFacesFrontCap)
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

		int facesFound = 0;
		// loop through each edge of the face and examine the adjacent face
		for(unsigned int ai = 0; ai < 3; ai++)
		{
			adjacentFaceIndex = face->AdjacentFaceIndex1;
			edgeVertex1 = faceVertex1;
			edgeVertex2 = faceVertex2;

			if(ai == 1)
			{
				adjacentFaceIndex = face->AdjacentFaceIndex2;
				edgeVertex1 = faceVertex2;
				edgeVertex2 = faceVertex3;
			}
			else if(ai == 2)
			{
				adjacentFaceIndex = face->AdjacentFaceIndex3;
				edgeVertex1 = faceVertex3;
				edgeVertex2 = faceVertex1;
			}

			float adjFaceToLightDot = 0;

			// adjacentFaceIndex >=0 means we found an adjacent face
			if(adjacentFaceIndex >= 0)
			{
				// adjacentFaceIndex will be the index in [faces] of the adjacent face
				facesFound++;
				adjacentFace = faces.GetFace(adjacentFaceIndex);
				if(adjacentFace != NULL)
				{
					// adjacentFaceVertexIndex is the index of the adjacent face's first vertex in [positions] & [normals]
					adjacentFaceVertexIndex = adjacentFace->FirstVertexIndex;

					// get the adjacent face's normal
					adjacentFaceNormal = normalsSource.GetVector(adjacentFaceVertexIndex);

					// copy the three vertices of the adjacent face to [adjVertex1], [adjVertex2], and [adjVertex3]
					adjVertex1 = positionsSrcPtr+(adjacentFaceVertexIndex*4);
					adjVertex2 = positionsSrcPtr+(adjacentFaceVertexIndex*4)+4;
					adjVertex3 = positionsSrcPtr+(adjacentFaceVertexIndex*4)+8;

					// compute average of three vertices
					vertexAvg.x = (adjVertex1[0] + adjVertex2[0] + adjVertex3[0]) / 3;
					vertexAvg.y = (adjVertex1[1] + adjVertex2[1] + adjVertex3[1]) / 3;
					vertexAvg.z = (adjVertex1[2] + adjVertex2[2] + adjVertex3[2]) / 3;

					// if we have a point light, use the average of the vertices to calculate the direction to it
					if(!directional)
					{
						adjFaceToLightDir.x = lightPosDir.x - vertexAvg.x;
						adjFaceToLightDir.y = lightPosDir.y - vertexAvg.y;
						adjFaceToLightDir.z = lightPosDir.z - vertexAvg.z;
						adjFaceToLightDir.Normalize();
					}

					// calculate dot product between adjacent face normal and direction to light
					adjFaceToLightDot = Vector3::Dot(adjFaceToLightDir, *adjacentFaceNormal);
				}
			}
			else adjFaceToLightDot = 1;

			if(backFacesFrontCap) // are we using back faces to build the volume front cap?
			{
				// if the current face is back facing, and either:
				//    1. The adjacent face on the current edge is front facing (adjFaceToLightDot >= backFaceThreshold)
				//    2. There is no adjacent face (adjacentFaceIndex < 0)
				//    3. [useBadGeometryShadowFix] == true
				// then we create two side polygons to link the current face, which will be a front cap triangle,
				// to the back cap triangle, which is the current face projected to infinity, on the current edge
				if(adjFaceToLightDot > backFaceThreshold || adjacentFaceIndex < 0 || useBadGeometryShadowFix)
				{
					BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
					BaseVector4_QuickCopy_IncDest(edgeVertex2, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

					BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex1, svPositionBase);

					currentPositionVertexIndex +=6;
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
				if(adjFaceToLightDot <= backFaceThreshold || adjacentFaceIndex < 0 || useBadGeometryShadowFix)
				{
					BaseVector4_QuickCopy_IncDest(edgeVertex2, svPositionBase);
					BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

					BaseVector4_QuickCopy_IncDest(edgeVertex1, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex1, svPositionBase);
					BaseVector4_QuickCopy_ZeroW_IncDest(edgeVertex2, svPositionBase);

					currentPositionVertexIndex +=6;
				}
			}
		}
	}
	shadowVolumePositions.SetCount(currentPositionVertexIndex);
}

/*
 * Set the vertex attribute buffer data for the shadow volume positions.
 */
void SubMesh3DRenderer::SetShadowVolumePositionData(Point3Array * points)
{
	attributeBuffers[(int)StandardAttribute::ShadowPosition]->SetData(points->GetDataPtr());
}

/*
 * Set the vertex attribute buffer data for the mesh vertex positions.
 */
void SubMesh3DRenderer::SetPositionData(Point3Array * points)
{
	attributeBuffers[(int)StandardAttribute::Position]->SetData(points->GetDataPtr());
}

/*
 * Set the vertex attribute buffer data for the mesh vertex normals.
 */
void SubMesh3DRenderer::SetNormalData(Vector3Array * normals)
{
	attributeBuffers[(int)StandardAttribute::Normal]->SetData(normals->GetDataPtr());
}

/*
 * Set the vertex attribute buffer data for the mesh vertex colors.
 */
void SubMesh3DRenderer::SetVertexColorData(Color4Array * colors)
{
	attributeBuffers[(int)StandardAttribute::VertexColor]->SetData(colors->GetDataPtr());
}

/*
 * Set the vertex attribute buffer data for the mesh UV coordinates set 1.
 */
void SubMesh3DRenderer::SetUV1Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture0]->SetData(uvs->GetDataPtr());
}

/*
 * Set the vertex attribute buffer data for the mesh UV coordinates set 2.
 */
void SubMesh3DRenderer::SetUV2Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture1]->SetData(uvs->GetDataPtr());
}

/*
 * Update the vertex attribute buffers of this sub-renderer to reflect type & size of the attribute
 * data in the target sub-mesh.
 */
bool SubMesh3DRenderer::UpdateMeshAttributeBuffers()
{
	// if vertex attribute buffers are already created, destroy them
	DestroyBuffers();
	ASSERT(containerRenderer != NULL,"SubMesh3DRenderer::UpdateMeshAttributeBuffers -> containerRenderer is NULL.",false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Could not find matching sub mesh for sub renderer.",false);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	StandardAttributeSet err = StandardAttributes::CreateAttributeSet();

	boundAttributeBuffers.clear();
	// loop through each standard attribute and create/initialize vertex attribute buffer for each
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			int componentCount = 4;
			if(attr == StandardAttribute::UVTexture0 || attr == StandardAttribute::UVTexture1)componentCount = 2;

			int stride =0;

			int initSuccess = InitAttributeData(attr, mesh->GetTotalVertexCount(), componentCount, stride);
			if(!initSuccess)StandardAttributes::AddAttribute(&err,attr);

			VertexAttrBufferBinding binding(attributeBuffers[i], attr, -1);
			boundAttributeBuffers.push_back(binding);
		}
	}

	// update the local vertex count
	storedVertexCount = mesh->GetTotalVertexCount();

	// TODO: current shadow volume data memory is allocated for all mesh renderers, regardless if they cast a
	// shadow or not. This could be quite wasteful, so implement a way to avoid this excess memory usage.
	bool shadowVolumeInitSuccess = true;
	shadowVolumeInitSuccess = shadowVolumePositions.Init(storedVertexCount * 8);
	shadowVolumeInitSuccess &= InitAttributeData(StandardAttribute::ShadowPosition, storedVertexCount * 8, 4,0);
	if(!shadowVolumeInitSuccess)
	{
		Debug::PrintError("SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Error occurred while initializing shadow volume array.");
		DestroyBuffers();
		return false;
	}

	boundShadowVolumeAttributeBuffers.clear();
	VertexAttrBufferBinding shadowVolumePositionBinding(attributeBuffers[(int)StandardAttribute::ShadowPosition], StandardAttribute::ShadowPosition, -1);
	boundShadowVolumeAttributeBuffers.push_back(shadowVolumePositionBinding);

	return true;
}

/*
 * If the attribute transformer for this sub-renderer is active, then it needs to allocate the
 * correct amount of space to store the transformed copies of those attributes. This method
 * accomplishes that.
 */
bool SubMesh3DRenderer::UpdateAttributeTransformerData()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::UpdateAttributeTransformerData -> Could not find matching sub mesh for sub renderer.",false);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	doPositionTransform = false;
	doNormalTransform = false;

	if(doAttributeTransform)
	{
		StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
		   StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
		{
			Point3Array * positions = mesh->GetPostions();
			unsigned int positionCount = positions->GetCount();
			if(positionCount != transformedPositions.GetCount())
			{
				if(!transformedPositions.Init(positionCount))
				{
					doAttributeTransform = false;
					doPositionTransform = false;
					doNormalTransform = false;
					Debug::PrintError("SubMesh3DRenderer::UpdateAttributeTransformerData -> Unable to init transformed positions array.");
					return false;
				}
			}

			doPositionTransform = true;
		}

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal) &&
		StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
		{
			Vector3Array * normals = mesh->GetVertexNormals();
			unsigned int normalCount = normals->GetCount();
			if(transformedVertexNormals.GetCount() != normalCount)
			{
				if(!transformedVertexNormals.Init(normalCount) || !transformedFaceNormals.Init(normalCount))
				{
					doAttributeTransform = false;
					doPositionTransform = false;
					doNormalTransform = false;
					Debug::PrintError("SubMesh3DRenderer::UpdateAttributeTransformerData -> Unable to transformed normals array.");
					return false;
				}
			}

			doNormalTransform = true;
		}
	}

	return true;
}

/*
 * Copy the attribute data from the target sub-mesh into the local vertex attribute buffers.
 */
void SubMesh3DRenderer::CopyMeshData()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::CopyMeshData -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::CopyMeshData -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetVertexNormals());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetVertexColorData(mesh->GetColors());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVsTexture0());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVsTexture1());
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
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::UpdateFromMesh -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::UpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

	bool updateSuccess = true;

	// if the vertex count of this sub-renderer does not match that of the target sub-mesh, call
	// the UpdateMeshAttributeBuffers() method to resize
	if(mesh->GetTotalVertexCount() != storedVertexCount || mesh->GetAttributeSet() != storedAttributes)
	{
		updateSuccess = updateSuccess && UpdateMeshAttributeBuffers();
	}

	// update this sub-renderer's attribute transformer so that its storage space for transformed vertex attributes
	// is large enough for the target sub-mesh
	updateSuccess = updateSuccess && UpdateAttributeTransformerData();

	ASSERT_RTRN(updateSuccess == true, "SubMesh3DRenderer::UpdateFromMesh -> Error occurred while updating mesh structure and data.");

	// copy over the data from the target sub-mesh
	CopyMeshData();
}

/*
 * Validate [material] to be used with this sub-renderer and its target sub-mesh. This means making sure that the attributes
 * expected by the shader belonging to [material] match the attributes that are supplied by the target sub-mesh. It also
 * means calling VerifySetVars() to ensure all uniforms & attributes expected by the shader have been set correctly.
 */
bool SubMesh3DRenderer::ValidateMaterialForMesh(MaterialRef material)
{
	// don't bother validating this material if it has already been validated
	if(material == lastUsedMaterial)return true;

	lastUsedMaterial = material;

	ASSERT(containerRenderer != NULL,"SubMesh3DRenderer::UseMaterial -> containerRenderer is NULL.", false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::UseMaterial -> Could not find matching sub mesh for sub renderer.", false);

	StandardAttributeSet materialAttributes = material->GetStandardAttributes();
	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	// look for mismatched shader variables and mesh attributes
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		if(StandardAttributes::HasAttribute(materialAttributes, attr))
		{
			if(!StandardAttributes::HasAttribute(meshAttributes, attr))
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
	if(attributeTransformer == NULL)
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
bool SubMesh3DRenderer::DoesAttributeTransform()
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
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::PreRender -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::PreRender -> Could not find matching sub mesh for sub renderer.");

	if(doAttributeTransform)
	{
		// pass the local->world-space transformation matrix and its inverse to the attribute transformer
		attributeTransformer->SetModelMatrix(model, modelInverse);

		if(doPositionTransform && doNormalTransform)
		{
			Point3Array * positions = mesh->GetPostions();
			Vector3Array * vertexNormals = mesh->GetVertexNormals();
			Vector3Array * faceNormals = mesh->GetFaceNormals();

			ASSERT_RTRN(positions != NULL && vertexNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL positions or normals.");
			ASSERT_RTRN(vertexNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL vertex normals.");
			ASSERT_RTRN(faceNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL face normals.");

			// invoke the attribute transformer
			attributeTransformer->TransformPositionsAndNormals(*positions, transformedPositions, *vertexNormals, transformedVertexNormals, *faceNormals, transformedFaceNormals, mesh->GetCenter(), transformedCenter);

			// update the positions vertex attribute buffer with transformed positions
			SetPositionData(&transformedPositions);

			// update the normals vertex attribute buffer with transformed normals
			SetNormalData(&transformedVertexNormals);
		}
		else
		{
			if(doPositionTransform)
			{
				Point3Array * positions = mesh->GetPostions();
				ASSERT_RTRN(positions != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL positions.");

				// invoke the attribute transformer
				attributeTransformer->TransformPositions(*positions, transformedPositions,  mesh->GetCenter(), transformedCenter);

				// update the positions vertex attribute buffer with transformed positions
				SetPositionData(&transformedPositions);
			}

			if(doNormalTransform)
			{
				Vector3Array * vertexNormals = mesh->GetVertexNormals();
				Vector3Array * faceNormals = mesh->GetFaceNormals();

				ASSERT_RTRN(vertexNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL vertex normals.");
				ASSERT_RTRN(faceNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL face normals.");

				// invoke the attribute transformer
				attributeTransformer->TransformNormals(*vertexNormals, transformedVertexNormals, *faceNormals, transformedFaceNormals);

				// update the normals vertex attribute buffer with transformed normals
				SetNormalData(&transformedVertexNormals);
			}
		}
	}
}

/*
 * Get the position of the center of the target sub-mesh after attribute transformation. If the
 * attribute transformer is not being used, then this value will be the same as the target
 * sub-mesh's existing center.
 */
const Point3* SubMesh3DRenderer::GetFinalCenter()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::Render -> Could not find matching sub mesh for sub renderer.", NULL);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(doAttributeTransform)
	{
		StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
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
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::Render -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

	MaterialRef currentMaterial = Engine::Instance()->GetGraphicsEngine()->GetActiveMaterial();
	ASSERT_RTRN(ValidateMaterialForMesh(currentMaterial), "SubMesh3DRendererGL::Render -> Invalid material for the current mesh.");

	Engine::Instance()->GetGraphicsEngine()->RenderTriangles(boundAttributeBuffers, mesh->GetTotalVertexCount(), true);
}

/*
 * Render the target sub-mesh's shadow volume.
 */
void SubMesh3DRenderer::RenderShadowVolume()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::RenderShadowVolume -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(targetSubMeshIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::RenderShadowVolume -> Could not find matching sub mesh for sub renderer.");

	if(shadowVolumePositions.GetCount() > 0)
	{
		// set the shadow volume vertex attribute buffer data
		SetShadowVolumePositionData(&shadowVolumePositions);

		// render shadow volume
		Engine::Instance()->GetGraphicsEngine()->RenderTriangles(boundShadowVolumeAttributeBuffers, shadowVolumePositions.GetCount(), false);
	}
}

/*
 * Set the [doBackSetShadowVolume] member boolean.
 */
void SubMesh3DRenderer::SetUseBackSetShadowVolume(bool use)
{
	doBackSetShadowVolume = use;
}

/*
 * Access [doBackSetShadowVolume] member boolean.
 */
bool SubMesh3DRenderer::GetUseBackSetShadowVolume()
{
	return doBackSetShadowVolume;
}
