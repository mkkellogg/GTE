#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "graphics/graphicsGL.h"
#include "graphics/shader/shaderGL.h"
#include "mesh3DrendererGL.h"
#include "mesh3Drenderer.h"
#include "mesh3D.h"
#include "graphics/graphics.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"
#include "ui/debug.h"

Mesh3DRendererGL::Mesh3DRendererGL() : Mesh3DRendererGL(false)
{

}

Mesh3DRendererGL::Mesh3DRendererGL(bool buffersOnGPU) : Mesh3DRenderer(buffersOnGPU)
{
	positionData = NULL;
	colorData = NULL;
	normalData = NULL;
	uv1Data = NULL;
	uv2Data = NULL;
}

Mesh3DRendererGL::~Mesh3DRendererGL()
{
	DestroyBuffers();
}

void Mesh3DRendererGL::DestroyBuffers()
{
	DestroyBuffer(&positionData);
	DestroyBuffer(&normalData);
	DestroyBuffer(&colorData);
	DestroyBuffer(&uv1Data);
	DestroyBuffer(&uv2Data);
}

void Mesh3DRendererGL::DestroyBuffer(VertexAttrBufferGL ** buffer)
{
	if(*buffer != NULL)delete *buffer;
	*buffer = NULL;
}


/*void Mesh3D::SetVertexData(const float * data, int componentCount, int count, AttributeType attributeType)
{
	switch(attributeType)
	{
		case AttributeType::Position:
			SetVertexData(positionData, data,componentCount,count,0);
		break;
		case AttributeType::Normal:
			SetVertexData(normalData, data,componentCount,count,0);
		break;
		case AttributeType::Color:
			SetVertexData(colorData, data,componentCount,count,0);
		break;
		case AttributeType::UV1:
			SetVertexData(uv1Data, data,componentCount,count,0);
		break;
		case AttributeType::UV2:
			SetVertexData(uv2Data, data,componentCount,count,0);
		break;
	}
}*/

void Mesh3DRendererGL::SetVertexData(VertexAttrBufferGL * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool Mesh3DRendererGL::InitBuffer(VertexAttrBufferGL ** buffer, int attributeCount, int componentCount)
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

	(*buffer)->Init(attributeCount, componentCount, buffersOnGPU, NULL);

	return true;
}

bool Mesh3DRendererGL::InitPositionData(int count)
{
	DestroyBuffer(&positionData);
	bool initSuccess = InitBuffer(&positionData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRendererGL::InitNormalData(int count)
{
	DestroyBuffer(&normalData);
	bool initSuccess = InitBuffer(&normalData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRendererGL::InitColorData(int count)
{
	DestroyBuffer(&colorData);
	bool initSuccess = InitBuffer(&colorData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRendererGL::InitUV1Data(int count)
{
	DestroyBuffer(&uv1Data);
	bool initSuccess = InitBuffer(&uv1Data, count, 2);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRendererGL::InitUV2Data(int count)
{
	DestroyBuffer(&uv2Data);
	bool initSuccess = InitBuffer(&uv2Data, count, 2);
	if(!initSuccess)return false;

	return true;
}

void Mesh3DRendererGL::SetPositionData(Point3Array * points)
{
	positionData->SetData(points->GetDataPtr());
}

void Mesh3DRendererGL::SetNormalData(Vector3Array * normals)
{
	normalData->SetData(normals->GetDataPtr());
}

void Mesh3DRendererGL::SetColorData(Color4Array * colors)
{
	colorData->SetData(colors->GetDataPtr());
}

void Mesh3DRendererGL::SetUV1Data(UV2Array * uvs)
{
	uv1Data->SetData(uvs->GetDataPtr());
}

void Mesh3DRendererGL::SetUV2Data(UV2Array * uvs)
{
	uv2Data->SetData(uvs->GetDataPtr());
}

bool Mesh3DRendererGL::UseMesh(Mesh3D * newMesh)
{
	mesh = NULL;

	DestroyBuffers();

	Mesh3DRenderer::UseMesh(newMesh);

	int errorMask = 0;

	AttributeSet meshAttributes = mesh->GetAttributeSet();

	if(Attributes::HasAttribute(meshAttributes, Attribute::Position))
	{
		int initSuccess = InitPositionData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMaskComponent::Position;
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::Normal))
	{
		int initSuccess = InitNormalData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMaskComponent::Normal;
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::Color))
	{
		int initSuccess = InitColorData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMaskComponent::Color;
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::UV1))
	{
		int initSuccess = InitUV1Data(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMaskComponent::UV1;
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::UV2))
	{
		int initSuccess = InitUV2Data(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMaskComponent::UV2;
	}

	if(errorMask != 0)
	{
		Mesh3DRenderer::UseMesh(NULL);
		char errorStr[64];
		sprintf(errorStr, "Error initializing attribute buffer(s) for Mesh3DRenderer: %d\n",errorMask);
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

	if(Attributes::HasAttribute(meshAttributes, Attribute::Position))
	{
		material->SendAttributeBufferToShader(Attribute::Position, positionData);
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::Normal))
	{
		material->SendAttributeBufferToShader(Attribute::Normal, normalData);
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::Color))
	{
		material->SendAttributeBufferToShader(Attribute::Color, colorData);
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::UV1))
	{
		material->SendAttributeBufferToShader(Attribute::UV1, uv1Data);
	}

	if(Attributes::HasAttribute(meshAttributes, Attribute::UV2))
	{
		material->SendAttributeBufferToShader(Attribute::UV2, uv2Data);
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
}

