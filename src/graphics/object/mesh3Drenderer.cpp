#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3Drenderer.h"
#include "graphics/graphics.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"
#include "ui/debug.h"

Mesh3DRenderer::Mesh3DRenderer() : Mesh3DRenderer(false)
{

}

Mesh3DRenderer::Mesh3DRenderer(bool buffersOnGPU)
{
	this->buffersOnGPU= buffersOnGPU;

	positionData = NULL;
	colorData = NULL;
	normalData = NULL;
	uv1Data = NULL;
	uv2Data = NULL;
}

Mesh3DRenderer::~Mesh3DRenderer()
{
	DestroyBuffers();
}

void Mesh3DRenderer::DestroyBuffers()
{
	DestroyBuffer(&positionData);
	DestroyBuffer(&normalData);
	DestroyBuffer(&colorData);
	DestroyBuffer(&uv1Data);
	DestroyBuffer(&uv2Data);
}

void Mesh3DRenderer::DestroyBuffer(VertexAttrBuffer ** buffer)
{
	Graphics * graphics = Graphics::Instance();
	if(*buffer != NULL)graphics->DestroyVertexAttrBuffer(*buffer);
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

void Mesh3DRenderer::SetVertexData(VertexAttrBuffer * buffer, const float * data, int componentCount, int totalCount, int stride)
{


}

bool Mesh3DRenderer::InitBuffer(VertexAttrBuffer ** buffer, int attributeCount, int componentCount)
{
	if(buffer == NULL)
	{
		Debug::PrintError("Attempted to initialize vertex attribute buffer from null pointer.");
		return false;
	}

	Graphics * graphics = Graphics::Instance();

	*buffer = graphics->CreateVertexAttrBuffer();

	if(*buffer == NULL)
	{
		Debug::PrintError("Graphics::CreateVertexAttrBuffer() returned NULL.");
		return false;
	}

	(*buffer)->Init(attributeCount, componentCount, buffersOnGPU, NULL);

	return true;
}

bool Mesh3DRenderer::InitPositionData(int count)
{
	DestroyBuffer(&positionData);
	bool initSuccess = InitBuffer(&positionData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRenderer::InitNormalData(int count)
{
	DestroyBuffer(&normalData);
	bool initSuccess = InitBuffer(&normalData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRenderer::InitColorData(int count)
{
	DestroyBuffer(&colorData);
	bool initSuccess = InitBuffer(&colorData, count, 4);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRenderer::InitUV1Data(int count)
{
	DestroyBuffer(&uv1Data);
	bool initSuccess = InitBuffer(&uv1Data, count, 2);
	if(!initSuccess)return false;

	return true;
}

bool Mesh3DRenderer::InitUV2Data(int count)
{
	DestroyBuffer(&uv2Data);
	bool initSuccess = InitBuffer(&uv2Data, count, 2);
	if(!initSuccess)return false;

	return true;
}


void Mesh3DRenderer::SetPositionData(Point3Array * points)
{
	positionData->SetData(points->GetDataPtr());
}

void Mesh3DRenderer::SetNormalData(Vector3Array * normals)
{
	normalData->SetData(normals->GetDataPtr());
}

void Mesh3DRenderer::SetColorData(Color4Array * colors)
{
	colorData->SetData(colors->GetDataPtr());
}

void Mesh3DRenderer::SetUV1Data(UV2Array * uvs)
{
	uv1Data->SetData(uvs->GetDataPtr());
}

void Mesh3DRenderer::SetUV2Data(UV2Array * uvs)
{
	uv2Data->SetData(uvs->GetDataPtr());
}

