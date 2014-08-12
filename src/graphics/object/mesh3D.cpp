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
	normalData = NULL;
	uv1Data = NULL;
	uv2Data = NULL;
}

Mesh3D::~Mesh3D()
{

}
void Mesh3D::SetVertexData(const float * data, AttributeType attributeType)
{
	switch(attributeType)
	{
		case AttributeType::Position:

		break;
		case AttributeType::Normal:

		break;
		case AttributeType::UV1:

		break;
		case AttributeType::UV2:

		break;
	}
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
