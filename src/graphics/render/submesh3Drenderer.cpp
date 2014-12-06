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
#include "ui/debug.h"

SubMesh3DRenderer::SubMesh3DRenderer(Graphics * graphics, AttributeTransformer * attributeTransformer) : SubMesh3DRenderer(false, graphics, attributeTransformer)
{

}

SubMesh3DRenderer::SubMesh3DRenderer(bool buffersOnGPU, Graphics * graphics, AttributeTransformer * attributeTransformer)
{
	this->graphics = graphics;
	this->containerRenderer = NULL;
	this->subIndex = -1;

	memset(attributeBuffers,0,sizeof(VertexAttrBuffer*) * MAX_ATTRIBUTE_BUFFERS);

	attributeBuffers[(int)StandardAttribute::Position]= NULL;
	attributeBuffers[(int)StandardAttribute::VertexColor]= NULL;

	storedVertexCount = 0;
	storedAttributes = StandardAttributes::CreateAttributeSet();

	this->buffersOnGPU = buffersOnGPU;
	this->attributeTransformer = attributeTransformer;

	doAttributeTransform = attributeTransformer == NULL ? false : true;
	doPositionTransform = false;
	doNormalTransform = false;
}

SubMesh3DRenderer::~SubMesh3DRenderer()
{
	Destroy();
}

void SubMesh3DRenderer::SetSubIndex(unsigned int index)
{
	subIndex = index;
}

void SubMesh3DRenderer::SetContainerRenderer(Mesh3DRenderer * renderer)
{
	ASSERT_RTRN(renderer != NULL, "SubMesh3DRenderer::SetContainerRenderer -> renderer is NULL");
	this->containerRenderer = renderer;
}

void SubMesh3DRenderer::Destroy()
{
	DestroyBuffers();
	SAFE_DELETE(attributeTransformer);
}

void SubMesh3DRenderer::DestroyBuffers()
{
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Position]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Normal]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::VertexColor]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture0]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture1]);
}

void SubMesh3DRenderer::DestroyBuffer(VertexAttrBuffer ** buffer)
{
	if((*buffer) != NULL)
	{
		graphics->DestroyVertexAttributeBuffer(*buffer);
	}
	*buffer = NULL;
}

void SubMesh3DRenderer::SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool SubMesh3DRenderer::InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount, int stride)
{
	ASSERT(buffer != NULL,"SubMesh3DRenderer::InitBuffer -> Attempted to initialize vertex attribute buffer from null pointer.", false);

	DestroyBuffer(buffer);

	*buffer = graphics->CreateVertexAttributeBuffer();
	ASSERT(*buffer != NULL,"SubMesh3DRenderer::InitBuffer -> Graphics::CreateVertexAttrBuffer() returned NULL.", false);

	(*buffer)->Init(vertexCount, componentCount, stride, buffersOnGPU, NULL);

	return true;
}

bool SubMesh3DRenderer::InitAttributeData(StandardAttribute attr, int componentCount,  int stride)
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	if(mesh.IsValid())
	{
		DestroyBuffer(&attributeBuffers[(int)attr]);
		bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], mesh->GetTotalVertexCount(), componentCount, stride);
		if(!initSuccess)return false;
	}
	return true;
}

void SubMesh3DRenderer::BuildShadowVolume(Vector3& lightPosDir, bool directional)
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(), "SubMesh3DRenderer::BuildShadowVolume -> mesh is invalid.");

    SubMesh3DFaces& faces = mesh->GetFaces();

	unsigned int faceCount = faces.GetFaceCount();

	unsigned int faceVertexIndex = 0;
	unsigned int adjacentFaceVertexIndex = 0;
	int adjacentFaceIndex = -1;
	Point3 * edgeV1;
	Point3 * edgeV2;

	unsigned int currentFrontFaceVertexIndex = 0;
	unsigned int currentBackFaceVertexIndex = 0;
	unsigned int currentSideVertexIndex = 0;

	float * svFrontBase = const_cast<float*>(shadowVolumeFront.GetDataPtr());

	Vector3 lightDirection = lightPosDir;
	Point3* vertex1;
	Point3* vertex2;
	Point3* vertex3;
	Vector3 faceToLightDir;

	Point3Array * positions = mesh->GetPostions();
	ASSERT_RTRN(positions, "SubMesh3DRenderer::BuildShadowVolume -> mesh contains NULL positions array.");
	Point3Array& positionsSource = doPositionTransform == true ? transformedPositions : *positions;
	float * positionsSrcPtr = const_cast<float*>(positionsSource.GetDataPtr());

	Vector3Array * normals = mesh->GetStraightNormals();
	ASSERT_RTRN(normals, "SubMesh3DRenderer::BuildShadowVolume -> mesh contains NULL straight normals array.");
	Vector3Array& normalsSource = doNormalTransform == true ? transformedStraightNormals : *normals;
	float * normalsSrcPtr = const_cast<float*>(normalsSource.GetDataPtr());

	SubMesh3DFace * face = NULL;
	SubMesh3DFace * adjacentFace = NULL;

	Vector3 * faceNormal;
	Vector3 * adjacentFaceNormal;

	for(unsigned int f = 0; f < faceCount; f++)
	{
		face = faces.GetFace(f);
		if(face == NULL)continue;

		faceVertexIndex = face->FirstVertexIndex;

		faceNormal = normalsSource.GetVector(faceVertexIndex);
		vertex1 = positionsSource.GetPoint(faceVertexIndex);
		vertex2 = positionsSource.GetPoint(faceVertexIndex+1);
		vertex3 = positionsSource.GetPoint(faceVertexIndex+2);

		if(!directional)
		{
			faceToLightDir.x = lightPosDir.x - vertex1->x;
			faceToLightDir.y = lightPosDir.y - vertex1->y;
			faceToLightDir.z = lightPosDir.z - vertex1->z;
		}
		else
		{
			faceToLightDir = lightPosDir;
			faceToLightDir.Invert();
		}

		float faceToLightDot = Vector3::Dot(faceToLightDir, *faceNormal);

		if(faceToLightDot > 0 )
		{
			float * vPtr = const_cast<float*>(vertex1->GetDataPtr());
			BaseVector4_QuickCopy(vPtr, svFrontBase);

			vPtr = const_cast<float*>(vertex2->GetDataPtr());
			BaseVector4_QuickCopy(vPtr, svFrontBase);

			vPtr = const_cast<float*>(vertex3->GetDataPtr());
			BaseVector4_QuickCopy(vPtr, svFrontBase);

			currentFrontFaceVertexIndex+=3;
		}


		for(unsigned int ai = 0; ai < 3; ai++)
		{
			adjacentFaceIndex = -1;
			edgeV1 = NULL;
			edgeV2 = NULL;

			if(ai == 0 && face->AdjacentFaceIndex1 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex1;
				edgeV1 = vertex1;
				edgeV2 = vertex2;
			}
			else if(ai == 1 && face->AdjacentFaceIndex2 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex2;
				edgeV1 = vertex2;
				edgeV2 = vertex3;
			}
			else if(ai == 2 && face->AdjacentFaceIndex3 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex3;
				edgeV1 = vertex3;
				edgeV2 = vertex1;
			}

			if(adjacentFaceIndex >=0)
			{
				adjacentFace = faces.GetFace(adjacentFaceIndex);
				if(adjacentFace == NULL)continue;

				adjacentFaceVertexIndex = adjacentFace->FirstVertexIndex;
				adjacentFaceNormal = normalsSource.GetVector(adjacentFaceVertexIndex);

				if(!directional)
				{
					float dot = Vector3::Dot(*faceNormal, *adjacentFaceNormal);
				}
			}
		}
	}
}


void SubMesh3DRenderer::SetPositionData(Point3Array * points)
{
	VertexAttrBuffer * buf = attributeBuffers[(int)StandardAttribute::Position];
	const float * ptr = points->GetDataPtr();
	buf->SetData(ptr);
}

void SubMesh3DRenderer::SetNormalData(Vector3Array * normals)
{
	attributeBuffers[(int)StandardAttribute::Normal]->SetData(normals->GetDataPtr());
}

void SubMesh3DRenderer::SetVertexColorData(Color4Array * colors)
{
	attributeBuffers[(int)StandardAttribute::VertexColor]->SetData(colors->GetDataPtr());
}

void SubMesh3DRenderer::SetUV1Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture0]->SetData(uvs->GetDataPtr());
}

void SubMesh3DRenderer::SetUV2Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture1]->SetData(uvs->GetDataPtr());
}

bool SubMesh3DRenderer::UpdateMeshAttributeBuffers()
{
	DestroyBuffers();
	ASSERT(containerRenderer != NULL,"SubMesh3DRenderer::UpdateMeshAttributeBuffers -> containerRenderer is NULL.",false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Could not find matching sub mesh for sub renderer.",false);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	StandardAttributeSet err = StandardAttributes::CreateAttributeSet();

	if(err != 0)
	{
		std::string msg("SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Error initializing attribute buffer(s):  ");
		msg += std::to_string(err);
		Debug::PrintError(msg);
		DestroyBuffers();
		return false;
	}

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			int componentCount = 4;
			if(attr == StandardAttribute::UVTexture0 || attr == StandardAttribute::UVTexture1)componentCount = 2;
		//	if(attr == StandardAttribute::Normal)componentCount = 3;

			int stride =0;
		//	if(attr == StandardAttribute::Normal)stride = 1;

			int initSuccess = InitAttributeData(attr, componentCount, stride);
			if(!initSuccess)StandardAttributes::AddAttribute(&err,attr);
		}
	}

	storedVertexCount = mesh->GetTotalVertexCount();

	bool shadowVolumeInitSuccess = true;
	shadowVolumeInitSuccess = shadowVolumeFront.Init(storedVertexCount);
	shadowVolumeInitSuccess = shadowVolumeInitSuccess && shadowVolumeBack.Init(storedVertexCount);
	shadowVolumeInitSuccess = shadowVolumeInitSuccess && shadowVolumeSides.Init(storedVertexCount * 2);

	if(!shadowVolumeInitSuccess)
	{
		Debug::PrintError("SubMesh3DRenderer::UpdateMeshAttributeBuffers -> Error occurred while initializing shadow volume array.");
		DestroyBuffers();
		return false;
	}

	return true;
}

bool SubMesh3DRenderer::UpdateAttributeTransformerData()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
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
			Vector3Array * normals = mesh->GetNormals();
			unsigned int normalCount = normals->GetCount();
			if(transformedNormals.GetCount() != normalCount)
			{
				if(!transformedNormals.Init(normalCount) || !transformedStraightNormals.Init(normalCount))
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

void SubMesh3DRenderer::CopyMeshData()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::CopyMeshData -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::CopyMeshData -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetNormals());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetVertexColorData(mesh->GetColors());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVsTexture0());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVsTexture1());
}

void SubMesh3DRenderer::UpdateFromMesh()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::UpdateFromMesh -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::UpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

	bool updateSuccess = true;
	if(mesh->GetTotalVertexCount() != storedVertexCount || mesh->GetAttributeSet() != storedAttributes)
	{
		updateSuccess = updateSuccess && UpdateMeshAttributeBuffers();
	}

	updateSuccess = updateSuccess && UpdateAttributeTransformerData();

	if(!updateSuccess)
	{
		Debug::PrintError("SubMesh3DRenderer::UpdateFromMesh -> Error occurred while updating mesh structure and data.");
		return;
	}

	CopyMeshData();
}

bool SubMesh3DRenderer::UseMaterial(MaterialRef material)
{
	if(material == activeMaterial)return true;

	ASSERT(material.IsValid(), "SubMesh3DRenderer::UseMaterial -> material is NULL", false);
	this->activeMaterial = material;

	ASSERT(containerRenderer != NULL,"SubMesh3DRenderer::UseMaterial -> containerRenderer is NULL.", false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRenderer::UseMaterial -> Could not find matching sub mesh for sub renderer.", false);

	StandardAttributeSet materialAttributes = material->GetStandardAttributes();
	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		if(StandardAttributes::HasAttribute(materialAttributes, attr))
		{
			if(!StandardAttributes::HasAttribute(meshAttributes, attr))
			{
				std::string msg = std::string("Shader was expecting attribute") + StandardAttributes::GetAttributeName(attr) + std::string("but mesh does not have it.");
				Debug::PrintWarning(msg);
			}
		}
	}

	return true;
}

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

AttributeTransformer * SubMesh3DRenderer::GetAttributeTransformer()
{
	return attributeTransformer;
}

bool SubMesh3DRenderer::DoesAttributeTransform()
{
	return doAttributeTransform;
}

void SubMesh3DRenderer::PreRender(const Matrix4x4& model, const Matrix4x4& modelInverse)
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRenderer::PreRender -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRenderer::PreRender -> Could not find matching sub mesh for sub renderer.");

	if(doAttributeTransform)
	{
		attributeTransformer->SetModelMatrix(model, modelInverse);

		if(doPositionTransform && doNormalTransform)
		{
			Point3Array * positions = mesh->GetPostions();
			Vector3Array * normals = mesh->GetNormals();
			Vector3Array * straightNormals = mesh->GetStraightNormals();

			ASSERT_RTRN(positions != NULL && normals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL positions or normals.");
			ASSERT_RTRN(normals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL normals.");
			ASSERT_RTRN(straightNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL straight normals.");

			attributeTransformer->TransformPositionsAndNormals(*positions, transformedPositions, *normals, transformedNormals, *straightNormals, transformedStraightNormals, mesh->GetCenter(), transformedCenter);
			SetPositionData(&transformedPositions);
			SetNormalData(&transformedNormals);
		}
		else
		{
			if(doPositionTransform)
			{
				Point3Array * positions = mesh->GetPostions();
				ASSERT_RTRN(positions != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL positions.");

				attributeTransformer->TransformPositions(*positions, transformedPositions,  mesh->GetCenter(), transformedCenter);
				SetPositionData(&transformedPositions);
			}

			if(doNormalTransform)
			{
				Vector3Array * normals = mesh->GetNormals();
				Vector3Array * straightNormals = mesh->GetStraightNormals();

				ASSERT_RTRN(normals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL normals.");
				ASSERT_RTRN(straightNormals != NULL,"SubMesh3DRenderer::PreRender -> mesh contains NULL straight normals.");

				attributeTransformer->TransformNormals(*normals, transformedNormals, *straightNormals, transformedStraightNormals);
				SetNormalData(&transformedNormals);
			}
		}
	}
}

const Point3* SubMesh3DRenderer::GetFinalCenter()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
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
