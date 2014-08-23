#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3DrendererGL.h"
#include "vertexattrbufferGL.h"
#include "graphics/attributes.h"
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


Mesh3DRendererGL::Mesh3DRendererGL() : Mesh3DRendererGL(false)
{

}

Mesh3DRendererGL::Mesh3DRendererGL(bool buffersOnGPU) : Mesh3DRenderer(), buffersOnGPU(false)
{
	memset(attributeBuffers,0,sizeof(VertexAttrBufferGL*) * MAX_ATTRIBUTE_BUFFERS);
}

Mesh3DRendererGL::~Mesh3DRendererGL()
{
	DestroyBuffers();
}

void Mesh3DRendererGL::DestroyBuffers()
{
	DestroyBuffer(&attributeBuffers[(int)Attribute::Position]);
	DestroyBuffer(&attributeBuffers[(int)Attribute::Normal]);
	DestroyBuffer(&attributeBuffers[(int)Attribute::Color]);
	DestroyBuffer(&attributeBuffers[(int)Attribute::UV1]);
	DestroyBuffer(&attributeBuffers[(int)Attribute::UV2]);
}

void Mesh3DRendererGL::DestroyBuffer(VertexAttrBufferGL ** buffer)
{
	if((*buffer) != NULL)delete (*buffer);
	*buffer = NULL;
}


void Mesh3DRendererGL::SetVertexData(VertexAttrBufferGL * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool Mesh3DRendererGL::InitBuffer(VertexAttrBufferGL ** buffer, int vertexCount, int componentCount, int stride)
{
	if(buffer == NULL)
	{
		Debug::PrintError("Attempted to initialize vertex attribute buffer from null pointer.");
		return false;
	}

	*buffer = new VertexAttrBufferGL();

	if(*buffer == NULL)
	{
		Debug::PrintError("Graphics::CreateVertexAttrBuffer() returned NULL.");
		return false;
	}

	(*buffer)->Init(vertexCount, componentCount, stride, buffersOnGPU, NULL);

	return true;
}

bool Mesh3DRendererGL::InitAttributeData(Attribute attr, int componentCount,  int stride)
{
	DestroyBuffer(&attributeBuffers[(int)attr]);
	bool initSuccess = InitBuffer(&attributeBuffers[(int)attr], mesh->GetVertexCount(), componentCount, stride);
	if(!initSuccess)return false;
	return true;
}


void Mesh3DRendererGL::SetPositionData(Point3Array * points)
{
	attributeBuffers[(int)Attribute::Position]->SetData(points->GetDataPtr());
}

void Mesh3DRendererGL::SetNormalData(Vector3Array * normals)
{
	attributeBuffers[(int)Attribute::Normal]->SetData(normals->GetDataPtr());
}

void Mesh3DRendererGL::SetColorData(Color4Array * colors)
{
	attributeBuffers[(int)Attribute::Color]->SetData(colors->GetDataPtr());
}

void Mesh3DRendererGL::SetUV1Data(UV2Array * uvs)
{
	attributeBuffers[(int)Attribute::UV1]->SetData(uvs->GetDataPtr());
}

void Mesh3DRendererGL::SetUV2Data(UV2Array * uvs)
{
	attributeBuffers[(int)Attribute::UV2]->SetData(uvs->GetDataPtr());
}

bool Mesh3DRendererGL::UseMesh(Mesh3D * newMesh)
{
	mesh = NULL;

	DestroyBuffers();

	Mesh3DRenderer::UseMesh(newMesh);

	AttributeSet meshAttributes = mesh->GetAttributeSet();
	AttributeSet err = Attributes::CreateAttributeSet();

	for(int i=0; i<(int)Attribute::_Last; i++)
	{
		Attribute attr = (Attribute)i;
		if(Attributes::HasAttribute(meshAttributes, attr))
		{
			int componentCount = 4;
			if(attr == Attribute::UV1 || attr == Attribute::UV2)componentCount = 2;
			if(attr == Attribute::Normal)componentCount = 3;

			int stride =0;
			if(attr == Attribute::Normal)stride = 1;

			int initSuccess = InitAttributeData(attr, componentCount, stride);
			if(!initSuccess)Attributes::AddAttribute(&err,attr);
		}
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::Position))SetPositionData(mesh->GetPostions());
	if(Attributes::HasAttribute(meshAttributes, Attribute::Normal))SetNormalData(mesh->GetNormals());
	if(Attributes::HasAttribute(meshAttributes, Attribute::Color))SetColorData(mesh->GetColors());
	if(Attributes::HasAttribute(meshAttributes, Attribute::UV1))SetUV1Data(mesh->GetUVs1());
	if(Attributes::HasAttribute(meshAttributes, Attribute::UV2))SetUV2Data(mesh->GetUVs2());

	if(err != 0)
	{
		Mesh3DRenderer::UseMesh(NULL);
		char errorStr[64];
		sprintf(errorStr, "Error initializing attribute buffer(s) for Mesh3DRenderer: %d\n",err);
		Debug::PrintError(errorStr);
		DestroyBuffers();
		return false;
	}

	return true;
}

bool Mesh3DRendererGL::UseMaterial(Material * material)
{
	Mesh3DRenderer::UseMaterial(material);

	AttributeSet materialAttributes = material->GetAttributeSet();
	AttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)Attribute::_Last; i++)
	{
		Attribute attr = (Attribute)i;

		if(Attributes::HasAttribute(materialAttributes, attr))
		{
			if(!Attributes::HasAttribute(meshAttributes, attr))
			{
				char msg[64];
				sprintf(msg, "Shader was expecting attribute %s, but mesh does not have it.", Attributes::GetAttributeName(attr));
				Debug::PrintWarning(msg);
			}
		}
	}

	return true;
}

void Mesh3DRendererGL::Render()
{
	GraphicsGL * graphics = (GraphicsGL *)Graphics::Instance();

	graphics->ActivateMaterial(material);

	AttributeSet meshAttributes = mesh->GetAttributeSet();

	for(int i=0; i<(int)Attribute::_Last; i++)
	{
		Attribute attr = (Attribute)i;
		if(Attributes::HasAttribute(meshAttributes, attr))
		{
			material->SendAttributeBufferToShader(attr, attributeBuffers[i]);
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
}

