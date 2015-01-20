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

//forward declarations
class Point3;
class Vector3;
class color4;
class UV2;
class EngineObjectManager;
class SubMesh3DRenderer;

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
	unsigned int totalVertexCount;
	// this sub-mesh's position in the containing Mesh3D instance's list of sub-meshes.
	int subIndex;

	// structure that describes (among other things) the adjacency relationship that
	// faces of this mesh have with each other
	SubMesh3DFaces faces;

	// these arrays hold the actual attribute data for this mesh
    Point3Array positions;
    Vector3Array vertexNormals;
    Vector3Array faceNormals;
    Color4Array colors;
    UV2Array uvsTexture0;
    UV2Array uvsTexture1;

    // inter-face angle above which smoothing/average of vertex normals should
    // not occur
    int normalsSmoothingThreshold;
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

    void Destroy();

	protected :

    SubMesh3D();
    SubMesh3D(StandardAttributeSet attributes);
    virtual ~SubMesh3D();

    void CalculateFaceNormal(unsigned int faceIndex, Vector3& result) const;
    void FindAdjacentFaceIndex(unsigned int faceIndex, int& edgeA, int& edgeB, int& edgeC) const;
    void BuildFaces();

    void CalcSphereOfInfluence();
    void CalculateNormals(float smoothingThreshhold);
    void SetContainerMesh(Mesh3D * mesh);
    void SetSubIndex(int index);

	public:

    SubMesh3DFaces& GetFaces();

    const Point3& GetCenter() const;
    const Vector3& GetSphereOfInfluenceX() const;
    const Vector3& GetSphereOfInfluenceY() const;
    const Vector3& GetSphereOfInfluenceZ() const;
    void SetNormalsSmoothingThreshold(unsigned int threshhold);
    void Update();

    bool Init(unsigned int totalVertexCount);
    unsigned int GetTotalVertexCount() const;
    StandardAttributeSet GetAttributeSet() const ;

    Point3Array * GetPostions();
    Vector3Array * GetVertexNormals();
    Vector3Array * GetFaceNormals();
    Color4Array * GetColors();
    UV2Array * GetUVsTexture0();
    UV2Array * GetUVsTexture1();
};

#endif

