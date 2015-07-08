/*
 * class: SubMesh3D
 *
 * author: Mark Kellogg
 *
 * SubMesh3D encapsulates a single mesh object and holds all of its attributes
 * (vertex positions, vertex normals, UV coordinates, etc...). It is designed
 * to be attached to a single Mesh3D object via [containerMesh].
 */

#ifndef _GTE_SUBMESH3D_H_
#define _GTE_SUBMESH3D_H_

#include "mesh3D.h"
#include "object/sceneobjectcomponent.h"
#include "graphics/stdattributes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"
#include "submesh3Dfaces.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "global/global.h"
#include "object/enginetypes.h"

namespace GTE
{
	//forward declarations
	class Point3;
	class Vector3;
	class color4;
	class UV2;
	class EngineObjectManager;
	class SubMesh3DRenderer;

	class SubMesh3D : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;
		friend class Mesh3D;

		// the attributes for which this mesh contains data
		StandardAttributeSet attributeSet;
		// total number of vertices in this mesh
		UInt32 totalVertexCount;
		// this sub-mesh's position in the containing Mesh3D instance's list of sub-meshes.
		Int32 subIndex;

		// structure that describes (among other things) the adjacency relationship that
		// faces of this mesh have with each other
		SubMesh3DFaces faces;

		// these arrays hold the actual attribute data for this mesh
		Point3Array positions;
		Vector3Array vertexNormals;
		Vector3Array vertexTangents;
		Vector3Array faceNormals;
		Color4Array colors;
		UV2Array uvs0;
		UV2Array uvs1;

		Bool isDirty;
		// inter-face angle above which smoothing/average of vertex normals should
		// not occur
		Int32 normalsSmoothingThreshold;
		// invert normals every time they are calculated
		Bool invertNormals;
		// invert tangents every time they are calculated
		Bool invertTangents;
		// local center position for sub-mesh.
		Point3 center;
		// radius of the sphere of influence along the X-axis
		Vector3 sphereOfInfluenceX;
		// radius of the sphere of influence along the Y-axis
		Vector3 sphereOfInfluenceY;
		// radius of the sphere  of influence along the Z-axis
		Vector3 sphereOfInfluenceZ;

		// pointer to the containing MEsh3D object
		Mesh3D * containerMesh;

		// last time this mesh was modified
		Real timeStamp;

		// maps vertices to other equal vertices
		std::vector<UInt32>** vertexCrossMap;
		// should face-related data be calculated?
		Bool buildFaces;
		// should normals be calculated?
		Bool calculateNormals;
		// should tangents be calculated?
		Bool calculateTangents;

		SubMesh3D();
		SubMesh3D(StandardAttributeSet attributes);
		virtual ~SubMesh3D();
		
		void Destroy();
		void DestroyVertexCrossMap();
		Bool BuildVertexCrossMap();

		void CalculateFaceNormal(UInt32 faceIndex, Vector3& result) const;
		void CalculateNormals(Real smoothingThreshhold);
		void CalculateTangent(UInt32 vertexIndex, UInt32 rightIndex, UInt32 leftIndex, Vector3& result);
		void CalculateTangents(Real smoothingThreshhold);
		void FindAdjacentFaceIndex(UInt32 faceIndex, int& edgeA, int& edgeB, int& edgeC) const;
		Int32 FindCommonFace(UInt32 excludeFace, UInt32 vaIndex, UInt32 vbIndex) const;
		void BuildFaces();

		void CalcSphereOfInfluence();

		void SetContainerMesh(Mesh3D * mesh);
		void SetSubIndex(Int32 index);

		void ReverseAttributeComponentOrder();
		void InvertNormals();
		void InvertTangents();
		void UpdateTimeStamp();

	public:

		void SetDirty(Bool isDirty);
		Bool IsDirty() const;

		void SetCalculateNormals(Bool calculate);
		void SetCalculateTangents(Bool calculate);
		void SetBuildFaces(Bool build);
		Bool HasFaces() const;

		SubMesh3DFaces& GetFaces();

		const Point3& GetCenter() const;
		const Vector3& GetSphereOfInfluenceX() const;
		const Vector3& GetSphereOfInfluenceY() const;
		const Vector3& GetSphereOfInfluenceZ() const;
		void SetNormalsSmoothingThreshold(UInt32 threshhold);
		void Update();
		void QuickUpdate();

		Bool Init(UInt32 totalVertexCount);
		UInt32 GetTotalVertexCount() const;
		StandardAttributeSet GetAttributeSet() const;
		Real GetTimeStamp() const;

		Point3Array * GetPostions();
		Vector3Array * GetVertexNormals();
		Vector3Array * GetVertexTangents();
		Vector3Array * GetFaceNormals();
		Color4Array * GetColors();
		UV2Array * GetUVs0();
		UV2Array * GetUVs1();

		void SetInvertNormals(Bool invert);
		void SetInvertTangents(Bool invert);
	};
}

#endif

