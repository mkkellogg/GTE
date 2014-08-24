#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "mesh3D.h"
#include "graphics/attributes.h"
#include "graphics/graphics.h"

#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"

#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"

#include "ui/debug.h"

Mesh3D::Mesh3D() : Mesh3D (Attributes::CreateAttributeSet())
{

}

Mesh3D::Mesh3D(AttributeSet attributes) : SceneObjectComponent()
{
	attributeSet = attributes;
	vertexCount = 0;

	positions = new Point3Array();
	normals = new Vector3Array();
	colors = new Color4Array();
	uvs1 = new UV2Array();
	uvs2 = new UV2Array();
}

Mesh3D::~Mesh3D()
{
	Destroy();
}

void Mesh3D::Destroy()
{
	if(positions != NULL)
	{
		delete positions;
		positions = NULL;
	}

	if(normals != NULL)
	{
		delete normals;
		normals = NULL;
	}

	if(colors != NULL)
	{
		delete colors;
		colors = NULL;
	}

	if(uvs1 != NULL)
	{
		delete uvs1;
		uvs1 = NULL;
	}

	if(uvs2 != NULL)
	{
		delete uvs2;
		uvs2 = NULL;
	}
}

int Mesh3D::GetVertexCount()
{
	return vertexCount;
}

AttributeSet Mesh3D::GetAttributeSet()
{
	return attributeSet;
}

bool Mesh3D::Init(int vertexCount)
{
	this->vertexCount = vertexCount;

	bool initSuccess = true;
	int errorMask = 0;

	if(Attributes::HasAttribute(attributeSet,Attribute::Position))
	{
		initSuccess = positions->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)AttributeMaskComponent::Position;
	}

	if(Attributes::HasAttribute(attributeSet,Attribute::Normal))
	{
		initSuccess = normals->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)AttributeMaskComponent::Normal;
	}

	if(Attributes::HasAttribute(attributeSet,Attribute::Color))
	{
		initSuccess = colors->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)AttributeMaskComponent::Color;
	}

	if(Attributes::HasAttribute(attributeSet,Attribute::UV1))
	{
		initSuccess = uvs1->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)AttributeMaskComponent::UV1;
	}

	if(Attributes::HasAttribute(attributeSet,Attribute::UV2))
	{
		initSuccess = uvs2->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)AttributeMaskComponent::UV2;
	}
	if(!initSuccess)
	{
		char errStr[64];
		sprintf(errStr, "Error initializing attribute array(s) for Mesh3D: %d\n",errorMask);
		Debug::PrintError(errStr);
		Destroy();
		return false;
	}

	return true;
}


Point3Array * Mesh3D::GetPostions()
{
	return positions;
}

Vector3Array * Mesh3D::GetNormals()
{
	return normals;
}

Color4Array * Mesh3D::GetColors()
{
	return colors;
}

UV2Array * Mesh3D::GetUVs1()
{
	return uvs1;
}

UV2Array * Mesh3D::GetUVs2()
{
	return uvs2;
}

