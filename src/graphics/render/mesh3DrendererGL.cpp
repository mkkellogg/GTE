#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3DrendererGL.h"
#include "vertexattrbufferGL.h"
#include "graphics/stdattributes.h"
#include "graphics/graphicsGL.h"
#include "graphics/shader/shaderGL.h"
#include "mesh3Drenderer.h"
#include "material.h"
#include "graphics/object/mesh3D.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "ui/debug.h"


Mesh3DRendererGL::Mesh3DRendererGL(Graphics * graphics) : Mesh3DRendererGL(false, graphics)
{

}

Mesh3DRendererGL::Mesh3DRendererGL(bool buffersOnGPU, Graphics * graphics) : Mesh3DRenderer(graphics), buffersOnGPU(false)
{
	memset(attributeBuffers,0,sizeof(VertexAttrBuffer*) * MAX_ATTRIBUTE_BUFFERS);

	attributeBuffers[(int)StandardAttribute::Position]= NULL;
	attributeBuffers[(int)StandardAttribute::VertexColor]= NULL;

	storedVertexCount = 0;
	storedAttributes = StandardAttributes::CreateAttributeSet();
}

Mesh3DRendererGL::~Mesh3DRendererGL()
{
	DestroyBuffers();
}

void Mesh3DRendererGL::DestroyBuffers()
{
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Position]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::Normal]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::VertexColor]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture0]);
	DestroyBuffer(&attributeBuffers[(int)StandardAttribute::UVTexture1]);
}

void Mesh3DRendererGL::DestroyBuffer(VertexAttrBuffer ** buffer)
{
	if((*buffer) != NULL)delete (*buffer);
	*buffer = NULL;
}


void Mesh3DRendererGL::SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool Mesh3DRendererGL::InitBuffer(VertexAttrBuffer ** buffer, int vertexCount, int componentCount, int stride)
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

bool Mesh3DRendererGL::InitAttributeData(StandardAttribute attr, int componentCount,  int stride)
{
	DestroyBuffer(&attributeBuffers[(int)attr]);
	bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], mesh->GetVertexCount(), componentCount, stride);
	if(!initSuccess)return false;
	return true;
}


void Mesh3DRendererGL::SetPositionData(Point3Array * points)
{
	attributeBuffers[(int)StandardAttribute::Position]->SetData(points->GetDataPtr());
}

void Mesh3DRendererGL::SetNormalData(Vector3Array * normals)
{
	attributeBuffers[(int)StandardAttribute::Normal]->SetData(normals->GetDataPtr());
}

void Mesh3DRendererGL::SetColorData(Color4Array * colors)
{
	attributeBuffers[(int)StandardAttribute::VertexColor]->SetData(colors->GetDataPtr());
}

void Mesh3DRendererGL::SetUV1Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture0]->SetData(uvs->GetDataPtr());
}

void Mesh3DRendererGL::SetUV2Data(UV2Array * uvs)
{
	attributeBuffers[(int)StandardAttribute::UVTexture1]->SetData(uvs->GetDataPtr());
}

bool Mesh3DRendererGL::UseMesh(Mesh3D * newMesh)
{
	mesh = NULL;

	DestroyBuffers();

	Mesh3DRenderer::UseMesh(newMesh);
	mesh->SetRenderer(this);

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
		Mesh3DRenderer::UseMesh(NULL);
		char errorStr[64];
		sprintf(errorStr, "Error initializing attribute buffer(s) for Mesh3DRenderer: %d\n",err);
		Debug::PrintError(errorStr);
		DestroyBuffers();
		return false;
	}

	storedVertexCount = mesh->GetVertexCount();
	CopyMeshData();

	if(material != NULL)
	{
		return UseMaterial(material);
	}

	//storedAttributes = meshAttributes;

	return true;
}

void Mesh3DRendererGL::CopyMeshData()
{
	StandardAttributeSet meshAttributes = mesh->GetAttributeSet();
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Position))SetPositionData(mesh->GetPostions());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::Normal))SetNormalData(mesh->GetNormals());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::VertexColor))SetColorData(mesh->GetColors());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture0))SetUV1Data(mesh->GetUVsTexture0());
	if(StandardAttributes::HasAttribute(meshAttributes, StandardAttribute::UVTexture1))SetUV2Data(mesh->GetUVsTexture1());
}

void Mesh3DRendererGL::UpdateFromMesh()
{
	if(mesh != NULL)
	{
		if(mesh->GetVertexCount() != storedVertexCount || storedAttributes != mesh->GetAttributeSet())
		{
			UseMesh(mesh);
		}
		else
		{
			CopyMeshData();
		}
	}
}

bool Mesh3DRendererGL::UseMaterial(Material * material)
{
	if(material == activeMaterial)return true;

	Mesh3DRenderer::UseMaterial(material);

	if(mesh != NULL)
	{
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
	}

	return true;
}

void Mesh3DRendererGL::Render()
{
	Material * currentMaterial = graphics->GetActiveMaterial();
	UseMaterial(currentMaterial);

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

