#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <unordered_map>
#include <functional>
#include <vector>

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
#include "gtemath/gtemath.h"
#include "graphics/light/light.h"
#include "ui/debug.h"
#include "global/constants.h"

Mesh3D::Mesh3D() : Mesh3D (StandardAttributes::CreateAttributeSet())
{

}

Mesh3D::Mesh3D(StandardAttributeSet attributes) : SceneObjectComponent()
{
	attributeSet = attributes;
	vertexCount = 0;
	normalsSmoothingThreshold = 90;

	positions = new Point3Array();
	normals = new Vector3Array();
	colors = new Color4Array();
	uvsTexture0 = new UV2Array();
	uvsTexture1 = new UV2Array();

	renderer = NULL;

	lightCullType = LightCullType::SphereOfInfluence;
}

Mesh3D::~Mesh3D()
{
	Destroy();
}

void Mesh3D::CalcSphereOfInfluence()
{
	float maxX=0;
	float maxY=0;
	float maxZ=0;
	float minX=0;
	float minY=0;
	float minZ=0;

	for(unsigned int v=0; v < vertexCount; v++)
	{
		Point3 * point = positions->GetPoint(v);
		if(point->x > maxX || v == 0)maxX = point->x;
		if(point->x < minX || v == 0)minX = point->x;
		if(point->y > maxY || v == 0)maxY = point->y;
		if(point->y < minY || v == 0)minY = point->y;
		if(point->z > maxZ || v == 0)maxZ = point->z;
		if(point->z < minZ || v == 0)minZ = point->z;
	}

	float width = maxX-minX;
	float height = maxY-minY;
	float depth = maxZ-minZ;

	center.x = width/2 + minX;
	center.y = height/2 + minY;
	center.z = depth/2 + minZ;

	//printf("center: %f,%f,%f == %f,%f,%f == %f, %f, %f\n", center.x, center.y, center.z, maxX, maxY, maxZ, minX, minY,minZ);

	sphereOfInfluenceX.Set(width * .6125,0,0);
	sphereOfInfluenceY.Set(0,height * .6125, 0);
	sphereOfInfluenceZ.Set(0,0,depth * .6125);
}

void Mesh3D::CalculateNormals(float smoothingThreshhold)
{
	for(unsigned int v =0; v < vertexCount-2; v+=3)
	{
		Point3 pa = positions->GetPoint(v);
		Point3 pb = positions->GetPoint(v+1);
		Point3 pc = positions->GetPoint(v+2);

		Vector3 a,b,c;

		Point3::Subtract(&pb, &pa, &b);
		Point3::Subtract(&pc, &pa, &a);

		Vector3::Cross(&a, &b, &c);
		c.Normalize();

		normals->GetVector(v)->Set(c.x,c.y,c.z);
		normals->GetVector(v+1)->Set(c.x,c.y,c.z);
		normals->GetVector(v+2)->Set(c.x,c.y,c.z);
	}

	std::unordered_map<Point3, std::vector<Vector3*>*, Point3::Point3Hasher,Point3::Point3Eq> normalGroups;
	for(unsigned int v =0; v < vertexCount; v++)
	{
		Point3 * point = positions->GetPoint(v);

		if(normalGroups.find(*point) == normalGroups.end())
		{
			normalGroups[*point] = new std::vector<Vector3*>();
		}

		std::vector<Vector3*>* list = normalGroups[*point];
		Vector3 * normal = normals->GetVector(v);
		list->push_back(normal);
	}

	for(unsigned int v =0; v < vertexCount; v++)
	{
		Vector3 oNormal = *normals->GetVector(v);
		oNormal.Normalize();

		Point3 * point = positions->GetPoint(v);
		std::vector<Vector3*>* list = normalGroups[*point];

		Vector3 avg(0,0,0);
		float divisor = 0;
		for(unsigned int i=0; i < list->size(); i++)
		{
			Vector3 current = (*((*list)[i]));

			float dot = Vector3::Dot(&current, &oNormal);
			float angle = acos(dot);
			if(angle <0)angle = -angle;

			angle /= Constants::TwoPIOver360;

			if(angle < smoothingThreshhold)
			{
				avg.x += current.x;
				avg.y += current.y;
				avg.z += current.z;
				divisor++;
			}
		}

		if(divisor < 1)
	    {
			avg.x = oNormal.x;
			avg.y = oNormal.y;
			avg.z = oNormal.z;
	    }
		else
		{
			float scaleFactor = (float)1.0/divisor;
			avg.Scale(scaleFactor);
		}

		normals->GetVector(v)->Set(avg.x,avg.y,avg.z);
	}

	for(unsigned int v =0; v < vertexCount; v++)
	{
		Point3 * point = positions->GetPoint(v);
		std::vector<Vector3*>* list = normalGroups[*point];
		if(list != NULL)
		{
			delete list;
			normalGroups[*point] = NULL;
		}
	}
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

	if(uvsTexture0 != NULL)
	{
		delete uvsTexture0;
		uvsTexture0 = NULL;
	}

	if(uvsTexture1 != NULL)
	{
		delete uvsTexture1;
		uvsTexture1 = NULL;
	}
}

const Point3 * Mesh3D::GetCenter() const
{
	return (const Point3 *)(&center);
}

const Vector3 * Mesh3D::GetSphereOfInfluenceX() const
{
	return (const Vector3 *)(&sphereOfInfluenceX);
}

const Vector3 * Mesh3D::GetSphereOfInfluenceY() const
{
	return (const Vector3 *)(&sphereOfInfluenceY);
}

const Vector3 * Mesh3D::GetSphereOfInfluenceZ() const
{
	return (const Vector3 *)(&sphereOfInfluenceZ);
}

LightCullType Mesh3D::GetLightCullType() const
{
	return lightCullType;
}

void Mesh3D::Update()
{
	CalcSphereOfInfluence();
	CalculateNormals(normalsSmoothingThreshold);
	if(renderer != NULL)renderer->UpdateFromMesh();
}

unsigned int Mesh3D::GetVertexCount() const
{
	return vertexCount;
}

StandardAttributeSet Mesh3D::GetAttributeSet() const
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

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture0))
	{
		initSuccess = uvsTexture0->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UVTexture0;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture1))
	{
		initSuccess = uvsTexture1->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UVTexture1;
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

void Mesh3D::SetNormalsSmoothingThreshold(unsigned int threshhold)
{
	if(threshhold > 180)threshhold = 180;
	this->normalsSmoothingThreshold = threshhold;
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

UV2Array * Mesh3D::GetUVsTexture0()
{
	return uvsTexture0;
}

UV2Array * Mesh3D::GetUVsTexture1()
{
	return uvsTexture1;
}

void Mesh3D::SetRenderer(Mesh3DRenderer * renderer)
{
	this->renderer = renderer;
}



