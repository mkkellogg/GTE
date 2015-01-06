#ifndef _SUBMESH3D_H_
#define _SUBMESH3D_H_

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
	friend class EngineObjectManager;
	friend class Mesh3D;

	StandardAttributeSet attributeSet;
	unsigned int totalVertexCount;
	int subIndex;

	SubMesh3DFaces faces;

    Point3Array positions;
    Vector3Array normals;
    Vector3Array straightNormals;
    Color4Array colors;
    UV2Array uvsTexture0;
    UV2Array uvsTexture1;

    int normalsSmoothingThreshold;
    Vector3 sphereOfInfluenceX;
    Vector3 sphereOfInfluenceY;
    Vector3 sphereOfInfluenceZ;
    Point3 center;

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
    Vector3Array * GetNormals();
    Vector3Array * GetStraightNormals();
    Color4Array * GetColors();
    UV2Array * GetUVsTexture0();
    UV2Array * GetUVsTexture1();
};

#endif

