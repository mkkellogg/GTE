#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3Drenderer.h"
#include "mesh3D.h"
#include "graphics/graphics.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"
#include "geometry/point3array.h"
#include "geometry/vector3array.h"
#include "graphics/color4array.h"
#include "graphics/uv2array.h"
#include "ui/debug.h"



Mesh3D::Mesh3D(int type)
{
	typeMask = type;

	positions = NULL;
	normals = NULL;
	colors = NULL;
	uvs1 = NULL;
	uvs2 = NULL;
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
}

bool Mesh3D::Init(int attributeCount)
{
	bool initSuccess = false;
	int errorMask = 0;

	if(typeMask & (int)AttributeType::Position)
	{
		positions = new Point3Array(attributeCount);
		initSuccess = positions->Init() && initSuccess;
		if(!initSuccess)errorMask &= (int)AttributeType::Position;
	}

	if(typeMask & (int)AttributeType::Normal)
	{
		normals = new Vector3Array(attributeCount);
		initSuccess = normals->Init() && initSuccess;
		if(!initSuccess)errorMask &= (int)AttributeType::Normal;
	}

	if(typeMask & (int)AttributeType::Color)
	{
		colors = new Color4Array(attributeCount);
		initSuccess = colors->Init() && initSuccess;
		if(!initSuccess)errorMask &= (int)AttributeType::Color;
	}

	if(typeMask & (int)AttributeType::UV1)
	{
		uvs1 = new UV2Array(attributeCount);
		initSuccess = uvs1->Init() && initSuccess;
		if(!initSuccess)errorMask &= (int)AttributeType::UV1;
	}

	if(typeMask & (int)AttributeType::UV2)
	{
		uvs2 = new UV2Array(attributeCount);
		initSuccess = uvs2->Init() && initSuccess;
		if(!initSuccess)errorMask &= (int)AttributeType::UV2;
	}
	if(!initSuccess)
	{
		char errStr[64];
		sprintf(errStr, "Error initializing attribute array for Mesh3D: %d\n",errorMask);
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

