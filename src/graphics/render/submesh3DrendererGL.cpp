#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "submesh3DrendererGL.h"
#include "vertexattrbufferGL.h"
#include "graphics/stdattributes.h"
#include "graphics/graphicsGL.h"
#include "graphics/shader/shaderGL.h"
#include "submesh3Drenderer.h"
#include "material.h"
#include "graphics/object/submesh3D.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "object/sceneobject.h"
#include "global/global.h"
#include "ui/debug.h"


SubMesh3DRendererGL::SubMesh3DRendererGL(Graphics * graphics) : SubMesh3DRendererGL(false, graphics)
{

}

SubMesh3DRendererGL::SubMesh3DRendererGL(bool buffersOnGPU, Graphics * graphics) : SubMesh3DRenderer(graphics), buffersOnGPU(false)
{
	memset(attributeBuffers,0,sizeof(VertexAttrBuffer*) * MAX_ATTRIBUTE_BUFFERS);

	attributeBuffers[(int)StandardAttribute::Position]= NULL;
	attributeBuffers[(int)StandardAttribute::VertexColor]= NULL;

	storedVertexCount = 0;
	storedAttributes = StandardAttributes::CreateAttributeSet();
}

SubMesh3DRendererGL::~SubMesh3DRendererGL()
{
	DestroyBuffers();
}

void SubMesh3DRendererGL::DestroyBuffers()
{
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Position]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Normal]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::VertexColor]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture0]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture1]);
}

void SubMesh3DRendererGL::DestroyBuffer(VertexAttrBuffer ** buffer)
{
	if((*buffer) != NULL)delete (*buffer);
	*buffer = NULL;
}


void SubMesh3DRendererGL::SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool SubMesh3DRendererGL::InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount, int stride)
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

bool SubMesh3DRendererGL::InitAttributeData(StandardAttribute attr, int componentCount,  int stride)
{
	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	if(mesh != NULL)
	{
		DestroyBuffer(&attributeBuffers[(int)attr]);
		bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], mesh->GetVertexCount(), componentCount, stride);
		if(!initSuccess)return false;
	}
	return true;
}


void SubMesh3DRendererGL::SetPositionData(Point3Array * points)
{
	attributeBuffers[(int)StandardAttribute::Position]->SetData(points->GetDataPtr());
}

void SubMesh3DRendererGL::SetNormalData(Vector3Array * normals)
{
	attributeBuffers[(int)StandardAttribute::Normal]->SetData(normals->GetDataPtr());
}

void SubMesh3DRendererGL::SetColorData(Color4Array * colors)
{
	attributeBuffers[(int)StandardAttribute::VertexColor]->SetData(colors->GetDataPtr());
}

void SubMesh3DRendererGL::SetUV1Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture0]->SetData(uvs->GetDataPtr());
}

void SubMesh3DRendererGL::SetUV2Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture1]->SetData(uvs->GetDataPtr());
}

bool SubMesh3DRendererGL::UpdateMeshData()
{
	DestroyBuffers();

	NULL_CHECK(sceneObject,"Mesh3DRendererGL::UpdateMesh -> Scene object is NULL.", false);

	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	NULL_CHECK(mesh,"Mesh3DRendererGL::UseMesh -> Scene object returned NULL mesh.",false);

	SubMesh3DRenderer::UpdateMeshData();

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
		std::string msg("Error initializing attribute buffer(s) for Mesh3DRenderer: ");
		msg += std::to_string(err);
		Debug::PrintError(msg);
		DestroyBuffers();
		return false;
	}

	storedVertexCount = mesh->GetVertexCount();
	CopyMeshData();

	if(material != NULL)
	{
		return UseMaterial(material);
	}

	return true;
}

void SubMesh3DRendererGL::CopyMeshData()
{
	NULL_CHECK_RTRN(sceneObject,"Mesh3DRendererGL::CopyMeshData -> Scene object is NULL.");

	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	NULL_CHECK_RTRN(mesh,"Mesh3DRendererGL::CopyMeshData -> Scene object has NULL mesh.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetNormals());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetColorData(mesh->GetColors());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVsTexture0());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVsTexture1());
}

void SubMesh3DRendererGL::UpdateFromMesh()
{
	NULL_CHECK_RTRN(sceneObject,"Mesh3DRendererGL::UpdateFromMesh -> Scene object is NULL.");

	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	NULL_CHECK_RTRN(mesh,"Mesh3DRendererGL::UpdateFromMesh -> Scene object has NULL mesh.");

	if(mesh->GetVertexCount() != storedVertexCount || storedAttributes != mesh->GetAttributeSet())
	{
		UpdateMeshData();
	}
	else
	{
		CopyMeshData();
	}
}

bool SubMesh3DRendererGL::UseMaterial(Material * material)
{
	if(material == activeMaterial)return true;

	SubMesh3DRenderer::UseMaterial(material);

	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	NULL_CHECK(mesh,"Mesh3DRendererGL::UseMaterial -> Scene object has NULL mesh.", false);

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

void SubMesh3DRendererGL::Render()
{
	Material * currentMaterial = graphics->GetActiveMaterial();
	UseMaterial(currentMaterial);

	SubMesh3D * mesh = sceneObject->GetSubMesh3D();
	NULL_CHECK_RTRN(mesh,"Mesh3DRendererGL::Render -> Scene object has NULL mesh.");

	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;
		if(StandardAttributes::HasAttribute(meshAttributes, attr))
		{
			currentMaterial->SendStandardAttributeBufferToShader(attr, attributeBuffers[i]);
		}
	}

	if(!currentMaterial->VerifySetVars(mesh->GetVertexCount()))return;

	glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
}

