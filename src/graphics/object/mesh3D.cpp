#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3D.h"
#include "graphics/graphics.h"
#include "ui/debug.h"

Mesh3D::Mesh3D()
{
	positionData = NULL;
	colorData = NULL;
	normalData = NULL;
	uv1Data = NULL;
	uv2Data = NULL;
}

Mesh3D::~Mesh3D()
{
	DestroyBuffers();
}

void Mesh3D::DestroyBuffers()
{
	Graphics * graphics = Graphics::Instance();

	if(positionData != NULL)graphics->DestroyVertexAttrBuffer(positionData);
	positionData = NULL;

	if(normalData != NULL)graphics->DestroyVertexAttrBuffer(normalData);
	normalData = NULL;

	if(colorData != NULL)graphics->DestroyVertexAttrBuffer(colorData);
	colorData = NULL;

	if(uv1Data != NULL)graphics->DestroyVertexAttrBuffer(uv1Data);
	uv1Data = NULL;

	if(uv2Data != NULL)graphics->DestroyVertexAttrBuffer(uv2Data);
	uv2Data = NULL;
}

void Mesh3D::SetVertexData(const float * data, int count, AttributeType attributeType)
{
	switch(attributeType)
	{
		case AttributeType::Position:
			SetVertexData(data,3,count,0);
		break;
		case AttributeType::Normal:
			SetVertexData(data,3,count,0);
		break;
		case AttributeType::Color:
			SetVertexData(data,4,count,0);
		break;
		case AttributeType::UV1:
			SetVertexData(data,2,count,0);
		break;
		case AttributeType::UV2:
			SetVertexData(data,2,count,0);
		break;
	}
}

void Mesh3D::SetVertexData(const float * data, int componentCount, int totalCount, int stride)
{


}

bool Mesh3D::InitializeVertexAttrBuffer(VertexAttrBuffer ** buffer)
{
	if(buffer == NULL)
	{
		Debug::PrintError("Attempted to initialize vertex attribute buffer from null pointer.");
		return false;
	}

	Graphics * graphics = Graphics::Instance();

	*buffer = graphics->CreateVertexAttrBuffer();

	return true;
}

bool Mesh3D::CheckAndInitializeVertexAttrBuffer(VertexAttrBuffer ** buffer)
{
	// only initialize if it is not already initialized
	if(*buffer == NULL)
	{
		return InitializeVertexAttrBuffer(buffer);
	}

	return true;
}