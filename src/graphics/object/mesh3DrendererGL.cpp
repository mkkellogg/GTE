#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

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

bool Mesh3DRendererGL::UseMesh(Mesh3D * newMesh)
{
	mesh = NULL;

	DestroyBuffers();

	Mesh3DRenderer::UseMesh(newMesh);

	int errorMask = 0;

	if(mesh->GetAttributeMask() & (int)AttributeMask::Position)
	{
		int initSuccess = InitPositionData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMask::Position;
	}

	if(mesh->GetAttributeMask() & (int)AttributeMask::Normal)
	{
		int initSuccess = InitNormalData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMask::Normal;
	}

	if(mesh->GetAttributeMask() & (int)AttributeMask::Color)
	{
		int initSuccess = InitColorData(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMask::Color;
	}

	if(mesh->GetAttributeMask() & (int)AttributeMask::UV1)
	{
		int initSuccess = InitUV1Data(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMask::UV1;
	}

	if(mesh->GetAttributeMask() & (int)AttributeMask::UV2)
	{
		int initSuccess = InitUV2Data(mesh->GetVertexCount());
		if(!initSuccess)errorMask &= (int)AttributeMask::UV2;
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

void Mesh3DRendererGL::Render()
{

}

