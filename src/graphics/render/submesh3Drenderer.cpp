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
	int adjacentFaceIndex = -1;
	unsigned int v1Index = 0;
	unsigned int v2Index = 0;

	unsigned int currentFrontFaceVertexIndex = 0;
	unsigned int currentBackFaceVertexIndex = 0;
	unsigned int currentSideVertexIndex = 0;
	Vector3 lightDirection = lightPosDir;

	Point3 vertex1;
	Point3 vertex2;
	Point3 vertex3;

	Vector3 tempA;
	Vector3 tempB;
	Vector3 tempC;

	Point3Array * positions = mesh->GetPostions();
	ASSERT_RTRN(positions, "SubMesh3DRenderer::BuildShadowVolume -> mesh contains NULL positions array.");
	Point3Array& positionsSource = doPositionTransform == true ? positionsCopy : *positions;

	for(unsigned int f = 0; f < faceCount; f++)
	{
		SubMesh3DFace * face = faces.GetFace(f);
		faceVertexIndex = face->FirstVertexIndex;
		adjacentFaceIndex = -1;

		positionsSource.GetPoint(face->FirstVertexIndex);

		for(unsigned int ai = 0; ai < 3; ai++)
		{
			if(ai == 0 && face->AdjacentFaceIndex1 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex1;
				v1Index = 0;
				v2Index = 1;
			}
			else if(ai == 1 && face->AdjacentFaceIndex2 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex2;
				v1Index = 1;
				v2Index = 2;
			}
			else if(ai == 2 && face->AdjacentFaceIndex3 >= 0)
			{
				adjacentFaceIndex = (unsigned int)face->AdjacentFaceIndex3;
				v1Index = 2;
				v2Index = 0;
			}

			if(adjacentFaceIndex >=0)
			{
				const SubMesh3DFace * adjacentFace = faces.GetFace(adjacentFaceIndex);
				if(adjacentFace == NULL)continue;

				if(!directional)
				{
					//Vector3::Subtract()
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

bool SubMesh3DRenderer::UpdateMeshStorageBuffers()
{
	DestroyBuffers();
	ASSERT(containerRenderer != NULL,"SubMesh3DRendererGL::UpdateMeshData -> containerRenderer is NULL.",false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRendererGL::UpdateMeshData -> Could not find matching sub mesh for sub renderer.",false);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	StandardAttributeSet err = StandardAttributes::CreateAttributeSet();

	if(err != 0)
	{
		std::string msg("Error initializing attribute buffer(s) for SubMesh3DRendererGL: ");
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
		Debug::PrintError("SubMesh3DRenderer::UpdateData -> Error occurred while initializing shadow volume array.");
		DestroyBuffers();
		return false;
	}

	return true;
}

bool SubMesh3DRenderer::UpdateAttributeTransformerData()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRendererGL::UpdateAttributeTransformerData -> Could not find matching sub mesh for sub renderer.",false);

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
			if(positions->GetMaxCount() != positionsCopy.GetMaxCount())
			{
				unsigned int positionCount = positions->GetMaxCount();
				if(!positionsCopy.Init(positionCount) || !transformedPositions.Init(positionCount))
				{
					doAttributeTransform = false;
					doPositionTransform = false;
					doNormalTransform = false;
					Debug::PrintError("SubMesh3DRendererGL::UpdateMeshData -> Unable to init local positions copy.");
					return false;
				}
			}

			doPositionTransform = true;
			const Point3& center = mesh->GetCenter();
			centerCopy.Set(center.x,center.y,center.z);
		}

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal) &&
		StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
		{
			Vector3Array * normals = mesh->GetNormals();
			if(normals->GetMaxCount() != normalsCopy.GetMaxCount())
			{
				unsigned int normalCount = normals->GetMaxCount();
				if(!normalsCopy.Init(normalCount) || !transformedNormals.Init(normalCount))
				{
					doAttributeTransform = false;
					doPositionTransform = false;
					doNormalTransform = false;
					Debug::PrintError("SubMesh3DRendererGL::UpdateMeshData -> Unable to init local normals copy.");
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
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::CopyMeshData -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::CopyMeshData -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(doAttributeTransform)
	{
		if(doPositionTransform)
		{
			Point3Array * positions = mesh->GetPostions();
			if(positions->GetMaxCount() == positionsCopy.GetMaxCount())
			{
				positions->CopyTo(&positionsCopy);
			}
			else Debug::PrintWarning("SubMesh3DRenderer::CopyMeshData -> Cannot copy positions because vertex count differs.");
		}
		else SetPositionData(mesh->GetPostions());

		if(doNormalTransform)
		{
			Vector3Array * normals = mesh->GetNormals();
			if(normals->GetMaxCount() == normalsCopy.GetMaxCount())
			{
				normals->CopyTo(&normalsCopy);
			}
			else Debug::PrintWarning("SubMesh3DRenderer::CopyMeshData -> Cannot copy normals because vertex count differs.");
		}
		else SetNormalData(mesh->GetNormals());
	}
	else
	{
		if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
		if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetNormals());
	}

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetNormals());

	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetVertexColorData(mesh->GetColors());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVsTexture0());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVsTexture1());
}

void SubMesh3DRenderer::UpdateFromMesh()
{
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::UpdateFromMesh -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::UpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

	bool updateSuccess = true;
	if(mesh->GetTotalVertexCount() != storedVertexCount || mesh->GetAttributeSet() != storedAttributes)
	{
		updateSuccess = updateSuccess && UpdateMeshStorageBuffers();
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

	ASSERT(containerRenderer != NULL,"SubMesh3DRendererGL::UseMaterial -> containerRenderer is NULL.", false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRendererGL::UseMaterial -> Could not find matching sub mesh for sub renderer.", false);

	StandardAttributeSet materialAttributes = material->GetStandardAttributes();
	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		if(StandardAttributes::HasAttribute(materialAttributes, attr))
		{
			if(!StandardAttributes::HasAttribute(meshAttributes, attr))
			{
				char msg[64];
				sprintf(msg, "Shader was expecting attribute %s, but mesh does not have it.", StandardAttributes::GetAttributeName(attr));
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
	ASSERT_RTRN(containerRenderer != NULL,"SubMesh3DRendererGL::Render -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT_RTRN(mesh.IsValid(),"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.");

	if(doAttributeTransform)
	{
		attributeTransformer->SetModelMatrix(model, modelInverse);

		if(doPositionTransform && doNormalTransform)
		{
			attributeTransformer->TransformPositionsAndNormals(positionsCopy, transformedPositions,normalsCopy, transformedNormals, centerCopy, transformedCenter);
			SetPositionData(&transformedPositions);
			SetNormalData(&transformedNormals);
		}
		else
		{
			if(doPositionTransform)
			{
				attributeTransformer->TransformPositions(positionsCopy, transformedPositions,  centerCopy, transformedCenter);
				SetPositionData(&transformedPositions);
			}

			if(doNormalTransform)
			{
				attributeTransformer->TransformNormals(normalsCopy, transformedNormals);
				SetNormalData(&transformedNormals);
			}
		}
	}
}

const Point3* SubMesh3DRenderer::GetFinalCenter()
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	ASSERT(mesh.IsValid(),"SubMesh3DRendererGL::Render -> Could not find matching sub mesh for sub renderer.", NULL);

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
