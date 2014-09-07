#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "mesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/stdattributes.h"
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

Mesh3D::Mesh3D() : Mesh3D (StandardAttributes::CreateAttributeSet())
{

}

Mesh3D::Mesh3D(StandardAttributeSet attributes) : SceneObjectComponent()
{
	attributeSet = attributes;
	vertexCount = 0;

	positions = new Point3Array();
	normals = new Vector3Array();
	colors = new Color4Array();
	uvs1 = new UV2Array();
	uvs2 = new UV2Array();

	renderer = NULL;
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

unsigned int Mesh3D::GetVertexCount()
{
	return vertexCount;
}

StandardAttributeSet Mesh3D::GetAttributeSet()
{
	return attributeSet;
}

bool Mesh3D::Init(unsigned int vertexCount)
{
	this->vertexCount = vertexCount;

	bool initSuccess = true;
	int errorMask = 0;

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Position))
	{
		initSuccess = positions->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::Position;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Normal))
	{
		initSuccess = normals->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::Normal;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Color))
	{
		initSuccess = colors->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::Color;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UV1))
	{
		initSuccess = uvs1->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UV1;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UV2))
	{
		initSuccess = uvs2->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UV2;
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

void Mesh3D::CalculateNormals(float smoothingThreshhold)
{
	// TODO: implement smoothing!!

	for(unsigned int v =0; v < vertexCount-2; v+=3)
	{
		Point3 pa = positions->GetPoint(v);
		Point3 pb = positions->GetPoint(v+1);
		Point3 pc = positions->GetPoint(v+2);

		Vector3 a,b,c;

		Point3::Subtract(&pb, &pa, &b);
		Point3::Subtract(&pc, &pa, &a);

		Vector3::Cross(&a, &b, &c);

		normals->GetVector(v)->Set(c.x,c.y,c.z);
		normals->GetVector(v+1)->Set(c.x,c.y,c.z);
		normals->GetVector(v+2)->Set(c.x,c.y,c.z);
	}
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

void Mesh3D::SetRenderer(Mesh3DRenderer * renderer)
{
	this->renderer = renderer;
}

