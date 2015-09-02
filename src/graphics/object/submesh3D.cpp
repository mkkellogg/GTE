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
#include "global/assert.h"
#include "global/constants.h"
#include "util/time.h"
#include "debug/gtedebug.h"
#include "engine.h"

namespace GTE
{
	/*
	* Default constructor
	*/
	SubMesh3D::SubMesh3D() : SubMesh3D(StandardAttributes::CreateAttributeSet())
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

		containerMesh = nullptr;
		subIndex = -1;
		invertNormals = false;
		invertTangents = false;

		vertexCrossMap = nullptr;

		buildFaces = true;
		calculateNormals = true;
		calculateTangents = true;

		UpdateUpdateCount();
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
	void SubMesh3D::SetSubIndex(Int32 index)
	{
		subIndex = index;
	}

	/*
	 * For a given face in the sub-mesh specified by [faceIndex], calculate the face's
	 * normal and store the result in [result]. [faceIndex] will be the index of the
	 * face's first vertex in [positions], the next two will be at [faceIndex] + 1,
	 * and [faceIndex] + 2.
	 */
	void SubMesh3D::CalculateFaceNormal(UInt32 faceIndex, Vector3& result) const
	{
		NONFATAL_ASSERT(faceIndex < totalVertexCount - 2, "SubMesh3D::CalculateFaceNormal -> 'faceIndex' is out range.", true);

		Vector3 a, b, c;

		// get Point3 objects for each vertex
		const Point3 *pa = positions.GetPointConst(faceIndex);
		const Point3 *pb = positions.GetPointConst(faceIndex + 1);
		const Point3 *pc = positions.GetPointConst(faceIndex + 2);

		NONFATAL_ASSERT(pa != nullptr && pb != nullptr && pc != nullptr, "SubMesh3D::CalculateFaceNormal -> Mesh vertex array contains null points.", true);

		// form 2 vectors based on triangle's vertices
		Point3::Subtract(*pb, *pa, b);
		Point3::Subtract(*pc, *pa, a);

		// calculate cross product
		Vector3::Cross(a, b, c);
		c.Normalize();

		result.Set(c.x, c.y, c.z);
	}

	/*
	* Calculate vertex normals using the two incident edges to calculate the
	* cross product. For all triangles that share a given vertex,the method will
	* calculate the average normal for that vertex as long as the angle between
	* the un-averaged normals is less than [smoothingThreshhold]. [smoothingThreshhold]
	* is specified in degrees.
	*/
	void SubMesh3D::CalculateNormals(Real smoothingThreshhold)
	{
		if (!StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))return;

		// loop through each triangle in this mesh's vertices
		// and calculate normals for each
		for (UInt32 v = 0; v < totalVertexCount - 2; v += 3)
		{
			Vector3 normal;
			CalculateFaceNormal(v, normal);

			vertexNormals.GetVector(v)->Set(normal.x, normal.y, normal.z);
			vertexNormals.GetVector(v + 1)->Set(normal.x, normal.y, normal.z);
			vertexNormals.GetVector(v + 2)->Set(normal.x, normal.y, normal.z);

			faceNormals.GetVector(v)->Set(normal.x, normal.y, normal.z);
			faceNormals.GetVector(v + 1)->Set(normal.x, normal.y, normal.z);
			faceNormals.GetVector(v + 2)->Set(normal.x, normal.y, normal.z);
		}

		// This vector is used to store the calculated average normal for all equal vertices
		std::vector<Vector3> averageNormals;

		// loop through each vertex and lookup the associated list of
		// normals associated with that vertex, and then calculate the
		// average normal from that list.
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			// get existing normal for this vertex
			Vector3 oNormal;
			oNormal = *(faceNormals.GetVector(v));
			oNormal.Normalize();

			// retrieve the list of equal vertices for vertex [v]
			std::vector<UInt32>* listPtr = vertexCrossMap[v];
			NONFATAL_ASSERT(listPtr != nullptr, "SubMesh3D::CalculateNormals -> Null pointer to vertex group list.", true);

			Vector3 avg(0, 0, 0);
			Real divisor = 0;

			std::vector<UInt32>& list = *listPtr;

			// compute the cosine of the smoothing threshhold angle
			Real cosSmoothingThreshhold = (GTEMath::Cos(Constants::DegreesToRads * smoothingThreshhold));

			for (UInt32 i = 0; i < list.size(); i++)
			{
				UInt32 vIndex = list[i];
				Vector3 * currentPtr = faceNormals.GetVector(vIndex);
				Vector3 current = *currentPtr;
				current.Normalize();

				// calculate angle between the normal that exists for this vertex,
				// and the current normal in the list.
				Real dot = Vector3::Dot(current, oNormal);

				if (dot > cosSmoothingThreshhold)
				{
					avg.x += current.x;
					avg.y += current.y;
					avg.z += current.z;
					divisor++;
				}
			}

			// if divisor <= 1, then no valid normals were found to include in the average,
			// so just use the existing one
			if (divisor <= 1)
			{
				avg.x = oNormal.x;
				avg.y = oNormal.y;
				avg.z = oNormal.z;
			}
			else
			{
				Real scaleFactor = (Real)1.0 / divisor;
				avg.Scale(scaleFactor);
				//avg.Normalize();
			}

			averageNormals.push_back(avg);
		}

		// loop through each vertex and assign the average normal
		// calculated for that vertex
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			Vector3 avg = averageNormals[v];
			avg.Normalize();
			// set the normal for this vertex to the averaged normal
			vertexNormals.GetVector(v)->Set(avg.x, avg.y, avg.z);
		}

		if (invertNormals)InvertNormals();
	}

	/*
	* Calculate the tangent for the vertex at [vertexIndex] in member [positions].
	*
	* The two edges used in the calculation (e1 and e2) are formed from the three vertices: v0, v1, v2.
	*
	* v0 is the vertex at [vertexIndex] in [positions].
	* v2 is the vertex at [rightIndex] in [positions].
	* v1 is the vertex at [leftIndex] in [positions].
	*/
	void SubMesh3D::CalculateTangent(UInt32 vertexIndex, UInt32 rightIndex, UInt32 leftIndex, Vector3& result)
	{
		UV2Array * sourceUVs = &uvs0;

		UV2 * uv0 = sourceUVs->GetCoordinate(vertexIndex);
		UV2 * uv2 = sourceUVs->GetCoordinate(rightIndex);
		UV2 * uv1 = sourceUVs->GetCoordinate(leftIndex);

		Vector3 e1, e2;

		Point3 * p0 = positions.GetPoint(vertexIndex);
		Point3 * p2 = positions.GetPoint(rightIndex);
		Point3 * p1 = positions.GetPoint(leftIndex);

		Point3::Subtract(*p1, *p0, e1);
		Point3::Subtract(*p2, *p0, e2);

		Real u0, u1, u2;
		Real v0, v1, v2;

		u0 = uv0->u;
		u1 = uv1->u;
		u2 = uv2->u;

		v0 = uv0->v;
		v1 = uv1->v;
		v2 = uv2->v;

		Real du1 = u1 - u0;
		Real du2 = u2 - u0;

		Real dv1 = v1 - v0;
		Real dv2 = v2 - v0;

		Real ood = 1.0f / ((du1 * dv2) - (du2 * dv1));

		result.Set(dv2*e1.x - dv1*e2.x, dv2*e1.y - dv1*e2.y, dv2*e1.z - dv1*e2.z);

		result.Scale(ood);
	}

	/*
	* Calculate vertex tangents using the two incident edges of a given vertex.
	* For all triangles that share a given vertex,the method will
	* calculate the average tangent for that vertex as long as the angle between
	* the un-averaged normals for the same vertices is less than [smoothingThreshhold].
	* [smoothingThreshhold is specified in degrees.
	*/
	void SubMesh3D::CalculateTangents(Real smoothingThreshhold)
	{
		if (!StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Tangent))return;

		// loop through each triangle in this mesh's vertices
		// and calculate tangents for each
		for (UInt32 v = 0; v < totalVertexCount - 2; v += 3)
		{
			Vector3 t0, t1, t2;

			CalculateTangent(v, v + 2, v + 1, t0);
			CalculateTangent(v + 1, v, v + 2, t1);
			CalculateTangent(v + 2, v + 1, v, t2);

			vertexTangents.GetVector(v)->SetTo(t0);
			vertexTangents.GetVector(v + 1)->SetTo(t1);
			vertexTangents.GetVector(v + 2)->SetTo(t2);
		}

		// This vector is used to store the calculated average tangent for all equal vertices
		std::vector<Vector3> averageTangents;

		// loop through each vertex and lookup the associated list of
		// tangents associated with that vertex, and then calculate the
		// average tangents from that list.
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			// get existing normal for this vertex
			Vector3 oNormal;
			oNormal = *(faceNormals.GetVector(v));
			oNormal.Normalize();

			Vector3 oTangent;
			oTangent = *(vertexTangents.GetVector(v));
			oTangent.Normalize();

			// retrieve the list of equal vertices for vertex [v]
			std::vector<UInt32>* listPtr = vertexCrossMap[v];
			NONFATAL_ASSERT(listPtr != nullptr, "SubMesh3D::CalculateNormals -> Null pointer to vertex group list.", true);

			Vector3 avg(0, 0, 0);
			Real divisor = 0;

			std::vector<UInt32>& list = *listPtr;

			// compute the cosine of the smoothing threshhold angle
			Real cosSmoothingThreshhold = (GTEMath::Cos(Constants::DegreesToRads * smoothingThreshhold));

			for (UInt32 i = 0; i < list.size(); i++)
			{
				UInt32 vIndex = list[i];
				Vector3 * currentPtr = faceNormals.GetVector(vIndex);
				Vector3 current = *currentPtr;
				current.Normalize();

				// calculate angle between the normal that exists for this vertex,
				// and the current normal in the list.
				Real dot = Vector3::Dot(current, oNormal);

				if (dot > cosSmoothingThreshhold)
				{
					Vector3 * tanPtr = vertexTangents.GetVector(vIndex);
					Vector3 tangent = *tanPtr;

					avg.x += tangent.x;
					avg.y += tangent.y;
					avg.z += tangent.z;
					divisor++;
				}
			}

			// if divisor < 1, then no extra tangents were found to include in the average,
			// so just use the original one

			if (divisor <= 1)
			{
				avg.x = oTangent.x;
				avg.y = oTangent.y;
				avg.z = oTangent.z;
			}
			else
			{
				Real scaleFactor = (Real)1.0 / divisor;
				avg.Scale(scaleFactor);
				//avg.Normalize();
			}

			averageTangents.push_back(avg);
		}

		// loop through each vertex and assign the average tangent
		// calculated for that vertex
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			Vector3 avg = averageTangents[v];
			avg.Normalize();
			// set the tangent for this vertex to the averaged tangent
			vertexTangents.GetVector(v)->Set(avg.x, avg.y, avg.z);
		}

		if(invertTangents)InvertTangents();
	}

	/*
	 * For a given face in the sub-mesh specified by [faceIndex], find up to three adjacent faces, and
	 * store the index of the adjacency edge's first vertex in [edgeA], [edgeB], and [edgeC].
	 */
	void SubMesh3D::FindAdjacentFaceIndex(UInt32 faceIndex, int& edgeA, int& edgeB, int& edgeC) const
	{
		NONFATAL_ASSERT(faceIndex < faces.GetFaceCount(), "SubMesh3D::FindAdjacentFaceIndex -> 'faceIndex' is out range.", true);
		const SubMesh3DFace * face = faces.GetFaceConst(faceIndex);

		Int32 faceVertexIndex = face->FirstVertexIndex;

		Int32 aResult = FindCommonFace(faceIndex, (UInt32)faceVertexIndex, (UInt32)faceVertexIndex + 1);
		Int32 bResult = FindCommonFace(faceIndex, (UInt32)faceVertexIndex + 1, (UInt32)faceVertexIndex + 2);
		Int32 cResult = FindCommonFace(faceIndex, (UInt32)faceVertexIndex + 2, (UInt32)faceVertexIndex);

		if (aResult >= 0)edgeA = aResult;
		if (bResult >= 0)edgeB = bResult;
		if (cResult >= 0)edgeC = cResult;
	}

	/*
	 * Find the face to which vertices at [vaIndex] and [vbIndex] in member [positions] both belong, excluding
	 * the face specified by [excludeFace].
	 */
	Int32 SubMesh3D::FindCommonFace(UInt32 excludeFace, UInt32 vaIndex, UInt32 vbIndex) const
	{
		std::vector<UInt32>* indicentVerticesA = vertexCrossMap[vaIndex];
		std::vector<UInt32>* indicentVerticesB = vertexCrossMap[vbIndex];

		NONFATAL_ASSERT_RTRN(indicentVerticesA != nullptr, "SubMesh3D::FindCommonFace -> 'indicentVerticesA' is null.", -1, true);
		NONFATAL_ASSERT_RTRN(indicentVerticesB != nullptr, "SubMesh3D::FindCommonFace -> 'indicentVerticesB' is null.", -1, true);

		for (UInt32 a = 0; a < indicentVerticesA->size(); a++)
		{
			UInt32 aFace = indicentVerticesA->operator [](a) / 3;
			for (UInt32 b = 0; b < indicentVerticesB->size(); b++)
			{
				UInt32 bFace = indicentVerticesB->operator [](b) / 3;

				if (aFace == bFace && aFace != excludeFace)return aFace;
			}
		}

		return -1;
	}

	/*
	 * Populate the [faces] data structure with each face in this mesh, and find
	 * the adjacent faces for each face.
	 */
	void SubMesh3D::BuildFaces()
	{
		UInt32 faceCount = faces.GetFaceCount();

		UInt32 vertexIndex = 0;
		for (UInt32 f = 0; f < faceCount; f++)
		{
			SubMesh3DFace * face = faces.GetFace(f);
			face->FirstVertexIndex = vertexIndex;
			vertexIndex += 3;
		}

		// loop through each face and call FindAdjacentFaceIndex() to find the
		// adjacent faces
		for (UInt32 f = 0; f < faceCount; f++)
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
		Real maxX, maxY, maxZ, minX, minY, minZ;
		maxX = maxY = maxZ = minX = minY = minZ = 0;

		// get the maximum and minimum extents of the mesh
		// along each axis.
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			Point3 * point = positions.GetPoint(v);
			if (point->x > maxX || v == 0)maxX = point->x;
			if (point->x < minX || v == 0)minX = point->x;
			if (point->y > maxY || v == 0)maxY = point->y;
			if (point->y < minY || v == 0)minY = point->y;
			if (point->z > maxZ || v == 0)maxZ = point->z;
			if (point->z < minZ || v == 0)minZ = point->z;
		}

		// get the dimensions of the rectangular volume formed by the
		// maximum extents
		Real width = maxX - minX;
		Real height = maxY - minY;
		Real depth = maxZ - minZ;

		// calculate the mesh's center
		center.x = width / 2 + minX;
		center.y = height / 2 + minY;
		center.z = depth / 2 + minZ;

		// form the sphere of influence vectors for each axis
		sphereOfInfluenceX.Set(width * .6125f, 0, 0);
		sphereOfInfluenceY.Set(0, height * .6125f, 0);
		sphereOfInfluenceZ.Set(0, 0, depth * .6125f);
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
		if (vertexCrossMap != nullptr)
		{
			std::unordered_map<long, Bool> deleted;
			for (UInt32 i = 0; i < totalVertexCount; i++)
			{
				std::vector<UInt32>* list = vertexCrossMap[i];
				if (list != nullptr && !deleted[(long)list])
				{
					delete list;
					deleted[(long)list] = true;
				}
			}

			delete vertexCrossMap;
			vertexCrossMap = nullptr;
		}
	}

	/*
	 * Construct [vertexCrossMap]. The vertex cross map is used to group all vertices that are equal.
	 * For a given combination of x,y,z, a corresponding list exists in [vertexCrossMap] with all the indices
	 * of vertices in [positions] which have a matching value for x,y, and z.
	 */
	Bool SubMesh3D::BuildVertexCrossMap()
	{
		// destroy existing cross map (if there is one).
		DestroyVertexCrossMap();

		// This map is used to link all equal vertices. Many triangles in a mesh can potentially have equal
		// vertices, so this structure is used to store indices in [positions] for those vertices.
		std::unordered_map<Point3, std::vector<UInt32>*, Point3::Point3Hasher, Point3::Point3Eq> vertexGroups;

		vertexCrossMap = new(std::nothrow) std::vector<UInt32>*[totalVertexCount];
		ASSERT(vertexCrossMap != nullptr, "SubMesh3D::BuildVertexCrossMap -> Could not allocate vertexCrossMap.");

		// loop through each vertex in the mesh and add the index in [position] for that vertex to the
		// appropriate vertex group.
		for (UInt32 v = 0; v < totalVertexCount; v++)
		{
			Point3 * point = positions.GetPoint(v);
			Point3 targetPoint = *point;

			std::vector<UInt32>*& list = vertexGroups[targetPoint];

			if (list == nullptr)list = new(std::nothrow) std::vector<UInt32>();

			// add the normal at index [v] to the vertex group linked to [targetPoint]
			list->push_back(v);
			vertexCrossMap[v] = list;
		}

		return true;
	}

	/*
	 * Tell this mesh whether or not to calculate its own normals.
	 */
	void SubMesh3D::SetCalculateNormals(Bool calculate)
	{
		calculateNormals = calculate;
	}

	/*
	 * Tell this mesh whether or not to calculate its own tangents.
	 */
	void SubMesh3D::SetCalculateTangents(Bool calculate)
	{
		calculateTangents = calculate;
	}

	/*
	 * Tell this mesh whether or not to build face data.
	 */
	void SubMesh3D::SetBuildFaces(Bool build)
	{
		buildFaces = build;
	}

	/*
	 * Does this mesh have face data?
	 */
	Bool SubMesh3D::HasFaces() const
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
		if (calculateNormals || calculateTangents || buildFaces)
		{
			if (!BuildVertexCrossMap())return;
		}

		CalcSphereOfInfluence();

		if (calculateNormals)CalculateNormals((GTE::Real)normalsSmoothingThreshold);
		if (calculateTangents)CalculateTangents((GTE::Real)normalsSmoothingThreshold);
		if (buildFaces)BuildFaces();

		if (containerMesh != nullptr)
		{
			containerMesh->CalculateSphereOfInfluence();
		}

		UpdateUpdateCount();
	}

	/*
	 * Only signal this mesh as updated, do not recalculate normals or faces. Useful
	 * when only changing vertex positions;
	 */
	void SubMesh3D::QuickUpdate()
	{
		UpdateUpdateCount();
	}
	/*
	 * Get the total number of vertices contained in this sub-mesh.
	 */
	UInt32 SubMesh3D::GetTotalVertexCount() const
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
	UInt32 SubMesh3D::GetUpdateCount() const
	{
		return updateCount;
	}

	/*
	 * Initialize this sub-mesh to contain space for [totalVertexCount] vertices.
	 */
	Bool SubMesh3D::Init(UInt32 totalVertexCount)
	{
		this->totalVertexCount = totalVertexCount;

		Bool initSuccess = true;
		Int32 errorMask = 0;

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Position))
		{
			initSuccess = positions.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::Position;
		}

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))
		{
			initSuccess = vertexNormals.Init(totalVertexCount) && initSuccess;
			initSuccess = faceNormals.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::Normal;
		}

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Tangent))
		{
			initSuccess = vertexTangents.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::Tangent;
		}

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::VertexColor))
		{
			initSuccess = colors.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::VertexColor;
		}

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::UVTexture0))
		{
			initSuccess = uvs0.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::UVTexture0;
		}

		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::UVTexture1))
		{
			initSuccess = uvs1.Init(totalVertexCount) && initSuccess;
			if (!initSuccess)errorMask |= (Int32)StandardAttributeMaskComponent::UVTexture1;
		}

		if (!initSuccess)
		{
			std::string msg = std::string("SubMesh3D::Init -> Error initializing attribute array(s) for SubMesh3D: ") + std::to_string(errorMask);
			Debug::PrintError(msg);
			Destroy();
			return false;
		}

		UInt32 faceCount = totalVertexCount / 3;
		Bool facesInitSuccess = faces.Init(faceCount);

		if (!facesInitSuccess)
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
		for (UInt32 i = 0; i < totalVertexCount; i += 3)
		{
			if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Position))
			{
				Point3 * p1 = positions.GetPoint(i);
				Point3  p1r = *p1;
				Point3 * p3 = positions.GetPoint(i + 2);

				*p1 = *p3;
				*p3 = p1r;
			}

			if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))
			{
				Vector3 * n1 = vertexNormals.GetVector(i);
				Vector3  n1r = *n1;
				Vector3 * n3 = vertexNormals.GetVector(i + 2);

				*n1 = *n3;
				*n3 = n1r;

				n1 = faceNormals.GetVector(i);
				n1r = *n1;
				n3 = faceNormals.GetVector(i + 2);

				*n1 = *n3;
				*n3 = n1r;
			}

			if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::VertexColor))
			{
				Color4 * c1 = colors.GetColor(i);
				Color4  c1r = *c1;
				Color4 * c3 = colors.GetColor(i + 2);

				*c1 = *c3;
				*c3 = c1r;
			}

			if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::UVTexture0))
			{
				UV2 * u1 = uvs0.GetCoordinate(i);
				UV2  u1r = *u1;
				UV2 * u3 = uvs0.GetCoordinate(i + 2);

				*u1 = *u3;
				*u3 = u1r;
			}

			if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::UVTexture1))
			{
				UV2 * u1 = uvs1.GetCoordinate(i);
				UV2  u1r = *u1;
				UV2 * u3 = uvs1.GetCoordinate(i + 2);

				*u1 = *u3;
				*u3 = u1r;
			}
		}
	}

	/*
	 * Indicate whether or not mesh normals should be inverted when they are calculated.
	 */
	void SubMesh3D::SetInvertNormals(Bool invert)
	{
		invertNormals = invert;
	}

	/*
	 * Indicate whether or not mesh tangents should be inverted when they are calculated.
	 */
	void SubMesh3D::SetInvertTangents(Bool invert)
	{
		invertTangents = invert;
	}

	/*
	 * Reverse the direction of all normals on this mesh
	 */
	void SubMesh3D::InvertNormals()
	{
		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Normal))
		{
			for (UInt32 i = 0; i < totalVertexCount; i++)
			{
				Vector3 * n1 = vertexNormals.GetVector(i);
				n1->Invert();
				n1 = faceNormals.GetVector(i);
				n1->Invert();
			}
		}
	}

	/*
	 * Reverse the direction of all tangents on this mesh
	 */
	void SubMesh3D::InvertTangents()
	{
		if (StandardAttributes::HasAttribute(attributeSet, StandardAttribute::Tangent))
		{
			for (UInt32 i = 0; i < totalVertexCount; i++)
			{
				Vector3 * n1 = vertexTangents.GetVector(i);
				n1->Invert();
			}
		}
	}

	/*
	 * Update the time this mesh was last modified.
	 */
	void SubMesh3D::UpdateUpdateCount()
	{
		updateCount++;
	}

	/*
	 * Set the threshold angle (in degrees) to be used when calculating averaged
	 * face normals for smoothed shading.
	 */
	void SubMesh3D::SetNormalsSmoothingThreshold(UInt32 threshhold)
	{
		if (threshhold > 180)threshhold = 180;
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
	 * Get the vertex normals for this sub-mesh. These may or may not
	 * be averaged for smooth shading.
	 */
	Vector3Array * SubMesh3D::GetVertexNormals()
	{
		return &vertexNormals;
	}

	/*
	 * Get the vertex tangents for this sub-mesh. These may or may not
	 * be averaged for smooth transitions between triangles.
	 */
	Vector3Array * SubMesh3D::GetVertexTangents()
	{
		return &vertexTangents;
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
	UV2Array * SubMesh3D::GetUVs0()
	{
		return &uvs0;
	}

	/*
	 * Get UV coordinates array 2 for this sub-mesh.
	 */
	UV2Array * SubMesh3D::GetUVs1()
	{
		return &uvs1;
	}
}

