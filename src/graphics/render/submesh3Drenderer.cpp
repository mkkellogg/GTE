#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
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
}

SubMesh3DRenderer::~SubMesh3DRenderer()
{
	DestroyBuffers();
	SAFE_DELETE(attributeTransformer);
}

void SubMesh3DRenderer::SetSubIndex(unsigned int index)
{
	subIndex = index;
}

void SubMesh3DRenderer::SetContainerRenderer(Mesh3DRenderer * renderer)
{
	NULL_CHECK_RTRN(renderer, "SubMesh3DRenderer::SetContainerRenderer -> renderer is NULL");
	this->containerRenderer = renderer;
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
	if(buffer == NULL)
	{
		Debug::PrintError("Attempted to initialize vertex attribute buffer from null pointer.");
		return false;
	}

	*buffer = graphics->CreateVertexAttributeBuffer();

	if(*buffer == NULL)
	{
		Debug::PrintError("Graphics::CreateVertexAttrBuffer() returned NULL.");
		return false;
	}

	(*buffer)->Init(vertexCount, componentCount, stride, buffersOnGPU, NULL);

	return true;
}

bool SubMesh3DRenderer::InitAttributeData(StandardAttribute attr, int componentCount,  int stride)
{
	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	if(mesh.IsValid())
	{
		DestroyBuffer(&attributeBuffers[(int)attr]);
		bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], mesh->GetVertexCount(), componentCount, stride);
		if(!initSuccess)return false;
	}
	return true;
}


void SubMesh3DRenderer::SetPositionData(Point3Array * points)
{
	attributeBuffers[(int)StandardAttribute::Position]->SetData(points->GetDataPtr());
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

bool SubMesh3DRenderer::UpdateMeshData()
{
	DestroyBuffers();
	NULL_CHECK(containerRenderer,"SubMesh3DRendererGL::UpdateMeshData -> containerRenderer is NULL.",false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	SHARED_REF_CHECK(mesh,"SubMesh3DRendererGL::UpdateMeshData -> Could not find matching sub mesh for sub renderer.",false);

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	StandardAttributeSet err = StandardAttributes::CreateAttributeSet();

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

	if(err != 0)
	{
		std::string msg("Error initializing attribute buffer(s) for SubMesh3DRendererGL: ");
		msg += std::to_string(err);
		Debug::PrintError(msg);
		DestroyBuffers();
		return false;
	}

	if(doAttributeTransform)
	{
		StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position) &&
		   StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
		{
			Point3Array * positions = mesh->GetPostions();
			if(positions->GetCount() != positionsCopy.GetCount())
			{
				unsigned int positionCount = positions->GetCount();
				if(!positionsCopy.Init(positionCount) || !transformedPositions.Init(positionCount))
				{
					doAttributeTransform = false;
					Debug::PrintError("SubMesh3DRendererGL::UpdateMeshData -> Unable to init local positions copy.");
					DestroyBuffers();
					return false;
				}
			}
		}

		if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal) &&
		StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
		{
			Vector3Array * normals = mesh->GetNormals();
			if(normals->GetCount() != normalsCopy.GetCount())
			{
				unsigned int normalCount = normals->GetCount();
				if(!normalsCopy.Init(normalCount) || !transformedNormals.Init(normalCount))
				{
					doAttributeTransform = false;
					Debug::PrintError("SubMesh3DRendererGL::UpdateMeshData -> Unable to init local normals copy.");
					DestroyBuffers();
					return false;
				}
			}
		}
	}

	storedVertexCount = mesh->GetVertexCount();
	CopyMeshData();

	return true;
}

void SubMesh3DRenderer::CopyMeshData()
{
	NULL_CHECK_RTRN(containerRenderer,"SubMesh3DRendererGL::CopyMeshData -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	SHARED_REF_CHECK_RTRN(mesh,"SubMesh3DRendererGL::CopyMeshData -> Could not find matching sub mesh for sub renderer.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	if(doAttributeTransform)
	{
		StandardAttributeSet attributesToTransform = attributeTransformer->GetActiveAttributes();

		if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))
		{
			if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Position))
			{
				Point3Array * positions = mesh->GetPostions();
				if(positions->GetCount() == positionsCopy.GetCount())
				{
					positions->CopyTo(&positionsCopy);
				}
			}
			else SetPositionData(mesh->GetPostions());
		}

		if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))
		{
			if(StandardAttributes::HasAttribute(attributesToTransform, StandardAttribute::Normal))
			{
				Vector3Array * normals = mesh->GetNormals();
				if(normals->GetCount() == normalsCopy.GetCount())
				{
					normals->CopyTo(&normalsCopy);
				}
			}
			else SetNormalData(mesh->GetNormals());
		}
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
	NULL_CHECK_RTRN(containerRenderer,"SubMesh3DRendererGL::UpdateFromMesh -> containerRenderer is NULL.");

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	SHARED_REF_CHECK_RTRN(mesh,"SubMesh3DRendererGL::UpdateFromMesh -> Could not find matching sub mesh for sub renderer.");

	if(mesh->GetVertexCount() != storedVertexCount || storedAttributes != mesh->GetAttributeSet())
	{
		UpdateMeshData();
	}
	else
	{
		CopyMeshData();
	}
}

bool SubMesh3DRenderer::UseMaterial(MaterialRef material)
{
	if(material == activeMaterial)return true;

	SHARED_REF_CHECK(material, "SubMesh3DRenderer::UseMaterial -> material is NULL", false);
	this->activeMaterial = material;

	NULL_CHECK(containerRenderer,"SubMesh3DRendererGL::UseMaterial -> containerRenderer is NULL.", false);

	SubMesh3DRef mesh = containerRenderer->GetSubMesh(subIndex);
	SHARED_REF_CHECK(mesh,"SubMesh3DRendererGL::UseMaterial -> Could not find matching sub mesh for sub renderer.", false);

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
