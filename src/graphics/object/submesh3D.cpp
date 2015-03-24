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
#include "submesh3Dface.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "graphics/graphics.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "graphics/light/light.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "gtemath/gtemath.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"
#include "global/constants.h"

/*
 * Default constructor
 */
SubMesh3D::SubMesh3D() : SubMesh3D (StandardAttributes::CreateAttributeSet())
{

}

/*
 * Constructor that allows you to specify the attributes this sub-mesh will hold.
 */
SubMesh3D::SubMesh3D(StandardAttributeSet attributes) : EngineObject()
{
	attributeSet = attributes;
	totalVertexCount = 0;
	normalsSmoothingThreshold = 90;

	containerMesh = NULL;
	subIndex = -1;
	invertNormals = false;

	vertexCrossMap = NULL;

	buildFaces = true;
	calculateNormals = true;

	UpdateTimeStamp();
}

/*
 * Clean-up
 */
SubMesh3D::~SubMesh3D()
{
	Destroy();
}

/*
 * Set the container mesh for this sub-mesh
 */
void SubMesh3D::SetContainerMesh(Mesh3D * containerMesh)
{
	this->containerMesh = containerMesh;
}

/*
 * Set this sub-mesh's position in the containing Mesh3D instance's list of sub-meshes.
 */
void SubMesh3D::SetSubIndex(int index)
{
	subIndex = index;
}

/*
 * For a given face in the sub-mesh specified by [faceIndex], calculate the face's
 * normal and store the result in [result]. [faceIndex] will be the index of the
 * face's first vertex in [positions], the next two will be at [faceIndex] + 1,
 * and [faceIndex] + 2.
 */
void SubMesh3D::CalculateFaceNormal(unsigned int faceIndex, Vector3& result) const
{
	ASSERT_RTRN(faceIndex < totalVertexCount - 2, "SubMesh3D::CalculateFaceNormal -> faceIndex is out range.");

	Vector3 a,b,c;

	// get Point3 objects for each vertex
	const Point3 *pa = positions.GetPointConst(faceIndex);
	const Point3 *pb = positions.GetPointConst(faceIndex+1);
	const Point3 *pc = positions.GetPointConst(faceIndex+2);

	ASSERT_RTRN(pa != NULL, "SubMesh3D::CalculateFaceNormal -> Mesh vertex array contains null points.");
	ASSERT_RTRN(pb != NULL, "SubMesh3D::CalculateFaceNormal -> Mesh vertex array contains null points.");
	ASSERT_RTRN(pc != NULL, "SubMesh3D::CalculateFaceNormal -> Mesh vertex array contains null points.");

	// form 2 vectors based on triangle's vertices
	Point3::Subtract(*pb, *pa, b);
	Point3::Subtract(*pc, *pa, a);

	// calculate cross product
	Vector3::Cross(a, b, c);
	c.Normalize();

	result.Set(c.x,c.y,c.z);
}

/*
 * For a given face in the sub-mesh specified by [faceIndex], find up to three adjacent faces, and
 * store the index of the adjacency edge's first vertex in [edgeA], [edgeB], and [edgeC].
 */
void SubMesh3D:: FindAdjacentFaceIndex(unsigned int faceIndex, int& edgeA, int& edgeB, int& edgeC) const
{
	ASSERT_RTRN(faceIndex < faces.GetFaceCount(), "SubMesh3D::FindAdjacentFaceIndex -> faceIndex is out range.");
	const SubMesh3DFace * face = faces.GetFaceConst(faceIndex);

	int faceVertexIndex = face->FirstVertexIndex;

	int aResult = FindCommonFace(faceIndex, (unsigned int)faceVertexIndex, (unsigned int)faceVertexIndex +1);
	int bResult = FindCommonFace(faceIndex, (unsigned int)faceVertexIndex+1, (unsigned int)faceVertexIndex +2);
	int cResult = FindCommonFace(faceIndex, (unsigned int)faceVertexIndex+2, (unsigned int)faceVertexIndex);

	if(aResult >= 0)edgeA = aResult;
	if(bResult >= 0)edgeB = bResult;
	if(cResult >= 0)edgeC = cResult;
}

/*
 * Find the face to which vertices at [vaIndex] and [vbIndex] in [positions] both belong, excluding
 * the face specified by [excludeFace].
 */
int SubMesh3D::FindCommonFace(unsigned int excludeFace, unsigned int vaIndex, unsigned int vbIndex) const
{
	std::vector<unsigned int>* indicentVerticesA = vertexCrossMap[vaIndex];
	std::vector<unsigned int>* indicentVerticesB = vertexCrossMap[vbIndex];

	for(unsigned int a =0; a < indicentVerticesA->size(); a++)
	{
		unsigned int aFace = indicentVerticesA->operator [](a) / 3;
		for(unsigned int b =0; b < indicentVerticesB->size(); b++)
		{
			unsigned int bFace = indicentVerticesB->operator [](b) / 3;

			if(aFace == bFace && aFace != excludeFace)return aFace;
		}
	}

	return -1;
}

/*
 * Populate the [faces] data structure, and find  the adjacent faces for each face.
 */
void SubMesh3D::BuildFaces()
{
	unsigned int faceCount = faces.GetFaceCount();

	unsigned int vertexIndex = 0;
	for(unsigned int f = 0; f < faceCount; f++)
	{
	    SubMesh3DFace * face = faces.GetFace(f);
		face->FirstVertexIndex = vertexIndex;
		vertexIndex += 3;
	}

	// loop through each face and call FindAdjacentFaceIndex() to find the
	// adjacent faces
	for(unsigned int f = 0; f < faceCount; f++)
	{
		SubMesh3DFace * face = faces.GetFace(f);
		FindAdjacentFaceIndex(f, face->AdjacentFaceIndex1, face->AdjacentFaceIndex2, face->AdjacentFaceIndex3);
	}
}

/*
 * Calculate the sphere of influence for this sub-mesh.
 */
void SubMesh3D::CalcSphereOfInfluence()
{
	float maxX,maxY,maxZ,minX,minY,minZ;
	maxX=maxY=maxZ=minX=minY=minZ=0;

	// get the maximum and minimum extents of the mesh
	// along each axis.
	for(unsigned int v=0; v < totalVertexCount; v++)
	{
		Point3 * point = positions.GetPoint(v);
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

/*
 * Calculate vertex normals using the two incident edges to calculate the
 * cross product. For all triangles that share a given vertex,the method will
 * calculate the average normal for that vertex as long as the difference between
 * the un-averaged normals is less than [smoothingThreshhold]. [smoothingThreshhold]
 * is specified in degrees.
 */
void SubMesh3D::CalculateNormals(float smoothingThreshhold)
{
	if(!StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))return;

	// loop through each triangle in this mesh's vertices
	// and calculate normals for each
	for(unsigned int v =0; v < totalVertexCount-2; v+=3)
	{
		Vector3 normal;
		CalculateFaceNormal(v, normal);

		vertexNormals.GetVector(v)->Set(normal.x,normal.y,normal.z);
		vertexNormals.GetVector(v+1)->Set(normal.x,normal.y,normal.z);
		vertexNormals.GetVector(v+2)->Set(normal.x,normal.y,normal.z);

		faceNormals.GetVector(v)->Set(normal.x,normal.y,normal.z);
		faceNormals.GetVector(v+1)->Set(normal.x,normal.y,normal.z);
		faceNormals.GetVector(v+2)->Set(normal.x,normal.y,normal.z);
	}

	// This vector is used to store the calculated average normal for all equal vertices
	std::vector<Vector3> averageNormals;

	// loop through each vertex and lookup the associated list of
	// normals associated with that vertex, and then calculate the
	// average normal from that list.
	for(unsigned int v =0; v < totalVertexCount; v++)
	{
		// get existing normal for this vertex
		Vector3 oNormal;
		oNormal = *(vertexNormals.GetVector(v));
		oNormal.Normalize();

		// get vertex position
		Point3 * point = positions.GetPoint(v);
		Point3 targetPoint = *point;

		// retrieve the list of normals for [targetPoint]
		std::vector<unsigned int>* listPtr = vertexCrossMap[v];
		ASSERT_RTRN(listPtr != NULL, "SubMesh3D::CalculateNormals -> NULL pointer to normal group list");

		Vector3 avg(0,0,0);
		float divisor = 0;
		for(unsigned int i=0; i < listPtr->size(); i++)
		{
			unsigned int vIndex = listPtr->operator[](i);
			Vector3 * currentPtr = vertexNormals.GetVector(vIndex);
			Vector3 current = *currentPtr;
			current.Normalize();

			// calculate angle between the normal that exists for this vertex,
			// and the current normal in the list.
			float dot = Vector3::Dot(current, oNormal);

			// clamp to the range -1.0 ... 1.0 to prevent rounding errors in ACos()
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
	for(unsigned int v =0; v < totalVertexCount; v++)
	{
		Vector3 avg = averageNormals[v];
		// set the normal for this vertex to the averaged normal
		vertexNormals.GetVector(v)->Set(avg.x,avg.y,avg.z);
	}

	if(invertNormals)InvertNormals();
}

/*
 * Deallocate all memory used by this sub-mesh.
 */
void SubMesh3D::Destroy()
{
	DestroyVertexCrossMap();
}

/*
 * Deallocate and destroy [vertexCrossMap].
 */
void SubMesh3D::DestroyVertexCrossMap()
{
	if(vertexCrossMap != NULL)
	{
		std::unordered_map<long, bool> deleted;
		for(unsigned int i = 0; i < totalVertexCount; i ++)
		{
			std::vector<unsigned int>* list =  vertexCrossMap[i];
			if(list != NULL && !deleted[(long)list])
			{
				delete list;
				deleted[(long)list] = true;
			}
		}

		delete vertexCrossMap;
		vertexCrossMap = NULL;
	}
}

/*
 * Construct [vertexCrossMap].
 */
bool SubMesh3D::BuildVertexCrossMap()
{
	DestroyVertexCrossMap();

	// This map is used to store normals for all equal vertices. Many triangles in a mesh can potentially have equal
	// vertices, so this structure is used to store all the normals for those vertices, so they can later
	// be used to calculate average (smoothed) normals
	std::unordered_map<Point3, std::vector<unsigned int>*, Point3::Point3Hasher,Point3::Point3Eq> normalGroups;

	vertexCrossMap = new std::vector<unsigned int>*[totalVertexCount];
	ASSERT(vertexCrossMap != NULL, "SubMesh3D::BuildVertexCrossMap -> Could not allocate vertexCrossMap.", false);

	// loop through each vertex in the mesh and store the normal for each in [normalGroups].
	// the normals for vertices that are equal (even if they are in different triangles) will be in the same list.
	for(unsigned int v = 0; v < totalVertexCount; v++)
	{
		Point3 * point = positions.GetPoint(v);
		Point3 targetPoint = *point;

		std::vector<unsigned int>*& list = normalGroups[targetPoint];

		if(list == NULL)list = new std::vector<unsigned int>();

		// add the normal at index [v] to the normal group linked to [targetPoint]
		list->push_back(v);
		vertexCrossMap[v] = list;
	}

	return true;
}

/*
 * Tell this mesh whether or not to calculate its own normals.
 */
void SubMesh3D::SetCalculateNormals(bool calculate)
{
	calculateNormals = calculate;
}

/*
 * Tell this mesh whether or not to build face data.
 */
void SubMesh3D::SetBuildFaces(bool build)
{
	buildFaces = build;
}

/*
 * Does this mesh have face data?
 */
bool SubMesh3D::HasFaces() const
{
	return buildFaces;
}

/*
 * Return the SubMesh3DFaces structure that describes the faces of this sub-mesh.
 */
SubMesh3DFaces& SubMesh3D::GetFaces()
{
	return faces;
}

/*
 * Get the center of this sub-mesh, which is calculated as the average of the
 * vertex positions.
 */
const Point3& SubMesh3D::GetCenter() const
{
	return center;
}

/*
 * Get the size of the sphere of influence along the local x-axis.
 */
const Vector3& SubMesh3D::GetSphereOfInfluenceX() const
{
	return sphereOfInfluenceX;
}

/*
 * Get the size of the sphere of influence along the local y-axis.
 */
const Vector3& SubMesh3D::GetSphereOfInfluenceY() const
{
	return sphereOfInfluenceY;
}

/*
 * Get the size of the sphere of influence along the local z-axis.
 */
const Vector3& SubMesh3D::GetSphereOfInfluenceZ() const
{
	return sphereOfInfluenceZ;
}

/*
 * Update any objects and data structures that are dependent on this sub-mesh's data.
 * Any time the attributes of this sub-mesh are updated, this method should be called.
 * It can be indirectly called by calling the Update() method of the containing Mesh3D
 * instance.
 */
void SubMesh3D::Update()
{
	if(calculateNormals || buildFaces)
	{
		if(!BuildVertexCrossMap())return;
	}
	CalcSphereOfInfluence();
	if(calculateNormals)CalculateNormals(normalsSmoothingThreshold);
	if(buildFaces)BuildFaces();

	if(containerMesh != NULL)
	{
		containerMesh->CalculateSphereOfInfluence();
	}

	UpdateTimeStamp();
}

/*
 * Only signal this mesh as updated, do not recalculate normals or faces. Useful
 * when only changing vertex positions;
 */
void SubMesh3D::QuickUpdate()
{
	UpdateTimeStamp();
}
/*
 * Get the total number of vertices contained in this sub-mesh.
 */
unsigned int SubMesh3D::GetTotalVertexCount() const
{
	return totalVertexCount;
}

/*
 * Get a StandardAttributeSet that describes the attributes possessed by
 * this sub-mesh.
 */
StandardAttributeSet SubMesh3D::GetAttributeSet() const
{
	return attributeSet;
}

/*
 * Get the time this mesh was last updated (in seconds since startup)
 */
float SubMesh3D::GetTimeStamp()
{
	return timeStamp;
}

/*
 * Initialize this sub-mesh to contain space for [totalVertexCount] vertices.
 */
bool SubMesh3D::Init(unsigned int totalVertexCount)
{
	this->totalVertexCount = totalVertexCount;

	bool initSuccess = true;
	int errorMask = 0;

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Position))
	{
		initSuccess = positions.Init(totalVertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::Position;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Normal))
	{
		initSuccess = vertexNormals.Init(totalVertexCount) && initSuccess;
		initSuccess = faceNormals.Init(totalVertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::Normal;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::VertexColor))
	{
		initSuccess = colors.Init(totalVertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::VertexColor;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture0))
	{
		initSuccess = uvsTexture0.Init(totalVertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UVTexture0;
	}

	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture1))
	{
		initSuccess = uvsTexture1.Init(totalVertexCount) && initSuccess;
		if(!initSuccess)errorMask |= (int)StandardAttributeMaskComponent::UVTexture1;
	}

	if(!initSuccess)
	{
		std::string msg = std::string("SubMesh3D::Init -> Error initializing attribute array(s) for SubMesh3D: ") + std::to_string(errorMask);
		Debug::PrintError(msg);
		Destroy();
		return false;
	}

	unsigned int faceCount = totalVertexCount / 3;
	bool facesInitSuccess = faces.Init(faceCount);

	if(!facesInitSuccess)
	{
		Debug::PrintError("SubMesh3D::Init -> Error occurred while initializing face array.");
		Destroy();
		return false;
	}

	return true;
}

/*
 * Reverse component order (i.e from CCW to CW or vice-versa)
 */
void SubMesh3D::ReverseAttributeComponentOrder()
{
	for(unsigned int i = 0; i< totalVertexCount; i += 3)
	{
		if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Position))
		{
			Point3 * p1 = positions.GetPoint(i);
			Point3  p1r = *p1;
			Point3 * p3 = positions.GetPoint(i+2);

			*p1 = *p3;
			*p3 = p1r;
		}

		if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Normal))
		{
			Vector3 * n1 = vertexNormals.GetVector(i);
			Vector3  n1r = *n1;
			Vector3 * n3 = vertexNormals.GetVector(i+2);

			*n1 = *n3;
			*n3 = n1r;

			n1 = faceNormals.GetVector(i);
			n1r = *n1;
			n3 = faceNormals.GetVector(i+2);

			*n1 = *n3;
			*n3 = n1r;
		}

		if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::VertexColor))
		{
			Color4 * c1 = colors.GetColor(i);
			Color4  c1r = *c1;
			Color4 * c3 = colors.GetColor(i+2);

			*c1 = *c3;
			*c3 = c1r;
		}

		if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture0))
		{
			UV2 * u1 = uvsTexture0.GetCoordinate(i);
			UV2  u1r = *u1;
			UV2 * u3 = uvsTexture0.GetCoordinate(i+2);

			*u1 = *u3;
			*u3 = u1r;
		}

		if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::UVTexture1))
		{
			UV2 * u1 = uvsTexture1.GetCoordinate(i);
			UV2  u1r = *u1;
			UV2 * u3 = uvsTexture1.GetCoordinate(i+2);

			*u1 = *u3;
			*u3 = u1r;
		}
	}
}

/*
 * Indicate whether or not mesh normals should be inverted when they are calculated.
 */
void SubMesh3D::SetInvertNormals(bool invert)
{
	invertNormals = invert;
}

/*
 * Reverse the direction of all normals on this mesh
 */
void SubMesh3D::InvertNormals()
{
	if(StandardAttributes::HasAttribute(attributeSet,StandardAttribute::Normal))
	{
		for(unsigned int i = 0; i< totalVertexCount; i++)
		{
			Vector3 * n1 = vertexNormals.GetVector(i);
			n1->Invert();
			n1 = faceNormals.GetVector(i);
			n1->Invert();
		}
	}
}

/*
 * Update the time this mesh was last modified.
 */
void SubMesh3D::UpdateTimeStamp()
{
	timeStamp = Time::GetRealTimeSinceStartup();
}

/*
 * Set the threshold angle (in degrees) to be used when calculating averaged
 * face normals for smoothed shading.
 */
void SubMesh3D::SetNormalsSmoothingThreshold(unsigned int threshhold)
{
	if(threshhold > 180)threshhold = 180;
	this->normalsSmoothingThreshold = threshhold;
}

/*
 * Get the vertex positions for this sub-mesh.
 */
Point3Array * SubMesh3D::GetPostions()
{
	return &positions;
}

/*
 * Get the vertex positions for this sub-mesh. These may or may not
 * be averaged for smooth shading.
 */
Vector3Array * SubMesh3D::GetVertexNormals()
{
	return &vertexNormals;
}

/*
 * Get the face normals for this sub-mesh. The face normals are the
 * same as the original un-averaged vertex normals.
 */
Vector3Array * SubMesh3D::GetFaceNormals()
{
	return &faceNormals;
}

/*
 * Get the vertex colors for this sub-mesh.
 */
Color4Array * SubMesh3D::GetColors()
{
	return &colors;
}

/*
 * Get UV coordinates array 1 for this sub-mesh.
 */
UV2Array * SubMesh3D::GetUVsTexture0()
{
	return &uvsTexture0;
}

/*
 * Get UV coordinates array 2 for this sub-mesh.
 */
UV2Array * SubMesh3D::GetUVsTexture1()
{
	return &uvsTexture1;
}

