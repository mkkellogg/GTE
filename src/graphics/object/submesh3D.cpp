#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "submesh3D.h"
#include "mesh3D.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "graphics/graphics.h"
#include "graphics/render/submesh3Drenderer.h"

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
#include "global/global.h"
#include "ui/debug.h"
#include "global/constants.h"

SubMesh3D::SubMesh3D() : SubMesh3D (StandardAttributes::CreateAttributeSet())
{

}

SubMesh3D::SubMesh3D(StandardAttributeSet attributes) : EngineObject()
{
	attributeSet = attributes;
	vertexCount = 0;
	normalsSmoothingThreshold = 90;

	positions = new Point3Array();
	normals = new Vector3Array();
	colors = new Color4Array();
	uvsTexture0 = new UV2Array();
	uvsTexture1 = new UV2Array();

	containerMesh = NULL;
	subIndex = -1;
}

SubMesh3D::~SubMesh3D()
{
	Destroy();
}

void SubMesh3D::SetContainerMesh(Mesh3D * containerMesh)
{
	this->containerMesh = containerMesh;
}

void SubMesh3D::SetSubIndex(int index)
{
	subIndex = index;
}

void SubMesh3D::CalcSphereOfInfluence()
{
	float maxX,maxY,maxZ,minX,minY,minZ;
	maxX=maxY=maxZ=minX=minY=minZ=0;

	// get the maximum and minimum extents of the mesh
	// along each axis.
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

	// get the dimensions of the rectangular volume formed by the
	// maximum extents
	float width = maxX-minX;
	float height = maxY-minY;
	float depth = maxZ-minZ;

	// calculate the mesh's center
	center.x = width/2 + minX;
	center.y = height/2 + minY;
	center.z = depth/2 + minZ;

	// form the sphere of influence vectors for each axis
	sphereOfInfluenceX.Set(width * .6125,0,0);
	sphereOfInfluenceY.Set(0,height * .6125, 0);
	sphereOfInfluenceZ.Set(0,0,depth * .6125);
}

void SubMesh3D::CalculateNormals(float smoothingThreshhold)
{
	if(!StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))return;

	// loop through each triangle in this mesh's vertices
	// and calculate normals for each
	for(unsigned int v =0; v < vertexCount-2; v+=3)
	{
		Vector3 a,b,c;

		// get Point3 objects for each vertex
		Point3 *pa = positions->GetPoint(v);
		Point3 *pb = positions->GetPoint(v+1);
		Point3 *pc = positions->GetPoint(v+2);

		ASSERT_RTRN(pa, "SubMesh3D::CalculateNormals -> Mesh vertex array contains null points.");
		ASSERT_RTRN(pb, "SubMesh3D::CalculateNormals -> Mesh vertex array contains null points.");
		ASSERT_RTRN(pc, "SubMesh3D::CalculateNormals -> Mesh vertex array contains null points.");

		// form 2 vectors based on triangle's vertices
		Point3::Subtract(*pb, *pa, b);
		Point3::Subtract(*pc, *pa, a);

		// calculate cross product
		Vector3::Cross(a, b, c);
		c.Normalize();

		normals->GetVector(v)->Set(c.x,c.y,c.z);
		normals->GetVector(v+1)->Set(c.x,c.y,c.z);
		normals->GetVector(v+2)->Set(c.x,c.y,c.z);
	}

	// This map is used to store normals for all equal vertices. Many triangles in a mesh can potentially have equal
	// vertices, so this structure is used to store all the normals for those vertices, so they can later
	// be used to calculate average (smoothed) normals
	std::unordered_map<Point3, std::shared_ptr<std::vector<Vector3*>>, Point3::Point3Hasher,Point3::Point3Eq> normalGroups;

	// This map is used to store the calculated average normal for all equal vertices
	//std::unordered_map<Point3, Vector3, Point3::Point3Hasher,Point3::Point3Eq> averageNormals;
	std::vector<Vector3> averageNormals;

	// loop through each vertex in the mesh and store the normal for each in [normalGroups].
	// the normals for vertices that are equal (even if they are in different triangles) will be in the same list.
	for(unsigned int v = 0; v < vertexCount; v++)
	{
		Point3 * point = positions->GetPoint(v);
		Point3 targetPoint = *point;

		// create a normal list for a vertex if one does not exist
		if(normalGroups.find(targetPoint) == normalGroups.end())
		{
			// we use a shared_ptr so that the vector will automatically be
			// deallocated when the containing map goes out of scope
			normalGroups[targetPoint] = std::shared_ptr<std::vector<Vector3*>>(new std::vector<Vector3*>());
		}

		std::shared_ptr<std::vector<Vector3*>> list = normalGroups[targetPoint];
		Vector3 * normal = normals->GetVector(v);
		list->push_back(normal);
	}

	// loop through each vertex and lookup the associated list of
	// normals associated with that vertex, and then calculate the
	// average normal from that list.
	for(unsigned int v =0; v < vertexCount; v++)
	{
		// get existing normal for this vertex
		Vector3 oNormal;
		oNormal = *(normals->GetVector(v));
		oNormal.Normalize();

		// get vertex position
		Point3 * point = positions->GetPoint(v);
		Point3 targetPoint = *point;

		std::shared_ptr<std::vector<Vector3*>> listPtr = normalGroups[targetPoint];

		ASSERT_RTRN(listPtr != NULL, "SubMesh3D::CalculateNormals -> NULL pointer to normal group list");
		std::vector<Vector3*> list = *listPtr;

		Vector3 avg(0,0,0);
		float divisor = 0;
		for(unsigned int i=0; i < list.size(); i++)
		{
			Vector3 * currentPtr = list[i];
			Vector3 current = *currentPtr;
			current.Normalize();

			// calculate angle between the normal that exists for this vertex,
			// and the current normal in the list.
			float dot = Vector3::Dot(current, oNormal);

			if (dot < -1.0) dot = -1.0 ;
			else if (dot > 1.0) dot = 1.0 ;

			float angle = GTEMath::ACos(dot);
			if(angle <0)angle = -angle;
			angle *= Constants::RadsToDegrees;
			if(angle < smoothingThreshhold)
			{
				avg.x += current.x;
				avg.y += current.y;
				avg.z += current.z;
				divisor++;
			}
		}

		// if divisor < 1, then no valid normals were found to include in the average,
		// so just use the existing one
		if(divisor <= 1)
		{
			avg.x = oNormal.x;
			avg.y = oNormal.y;
			avg.z = oNormal.z;
		}
		else
		{
			float scaleFactor = (float)1.0/divisor;
			avg.Scale(scaleFactor);
			//avg.Normalize();
		}

		averageNormals.push_back(avg);
	}

	// loop through each vertex and assign the average normal
	// calculated for that vertex
	for(unsigned int v =0; v < vertexCount; v++)
	{
		Vector3 avg = averageNormals[v];
		// set the normal for this vertex to the averaged normal
		normals->GetVector(v)->Set(avg.x,avg.y,avg.z);
	}
}

void SubMesh3D::Destroy()
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

const Point3 * SubMesh3D::GetCenter() const
{
	return (const Point3 *)(&center);
}

const Vector3 * SubMesh3D::GetSphereOfInfluenceX() const
{
	return (const Vector3 *)(&sphereOfInfluenceX);
}

const Vector3 * SubMesh3D::GetSphereOfInfluenceY() const
{
	return (const Vector3 *)(&sphereOfInfluenceY);
}

const Vector3 * SubMesh3D::GetSphereOfInfluenceZ() const
{
	return (const Vector3 *)(&sphereOfInfluenceZ);
}

void SubMesh3D::Update()
{
	CalcSphereOfInfluence();
	CalculateNormals(normalsSmoothingThreshold);

	if(containerMesh != NULL)
	{
		containerMesh->CalculateSphereOfInfluence();

		 if(containerMesh->IsAttachedToSceneObject() &&
			containerMesh->SceneObjectHasRenderer())
		{
			if(subIndex >=0)
				containerMesh->SendDataToRenderer((unsigned int)subIndex);
		}
	}
}

unsigned int SubMesh3D::GetVertexCount() const
{
	return vertexCount;
}

StandardAttributeSet SubMesh3D::GetAttributeSet() const
{
	return attributeSet;
}

bool SubMesh3D::Init(unsigned int vertexCount)
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

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::VertexColor))
	{
		initSuccess = colors->Init(vertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::VertexColor;
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
		sprintf(errStr, "Error initializing attribute array(s) for SubMesh3D: %d\n",errorMask);
		Debug::PrintError(errStr);
		Destroy();
		return false;
	}

	return true;
}

void SubMesh3D::SetNormalsSmoothingThreshold(unsigned int threshhold)
{
	if(threshhold > 180)threshhold = 180;
	this->normalsSmoothingThreshold = threshhold;
}

Point3Array * SubMesh3D::GetPostions()
{
	return positions;
}

Vector3Array * SubMesh3D::GetNormals()
{
	return normals;
}

Color4Array * SubMesh3D::GetColors()
{
	return colors;
}

UV2Array * SubMesh3D::GetUVsTexture0()
{
	return uvsTexture0;
}

UV2Array * SubMesh3D::GetUVsTexture1()
{
	return uvsTexture1;
}



