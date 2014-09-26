#ifndef _MESH3D_H_
#define _MESH3D_H_

//forward declarations
class Point3;
class Vector3;
class color4;
class UV2;
class Point3Array;
class Vector3Array;
class Color4Array;
class UV2Array;
class EngineObjectManager;
class Mesh3DRenderer;

#include "object/sceneobjectcomponent.h"
#include "graphics/stdattributes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/light/light.h"

class Mesh3D : public SceneObjectComponent
{
	friend EngineObjectManager;

	StandardAttributeSet attributeSet;
	unsigned int vertexCount;

    Point3Array * positions;
    Vector3Array * normals;
    Color4Array * colors;
    UV2Array * uvsTexture0;
    UV2Array * uvsTexture1;

    int normalsSmoothingThreshold;
    Vector3 sphereOfInfluenceX;
    Vector3 sphereOfInfluenceY;
    Vector3 sphereOfInfluenceZ;
    Point3 center;

    LightCullType lightCullType;

    void Destroy();

	protected :

    Mesh3D();
    Mesh3D(StandardAttributeSet attributes);
    virtual ~Mesh3D();
    void CalcSphereOfInfluence();
    void CalculateNormals(float smoothingThreshhold);

	public:

    const Point3 * GetCenter() const;
    const Vector3 * GetSphereOfInfluenceX() const;
    const Vector3 * GetSphereOfInfluenceY() const;
    const Vector3 * GetSphereOfInfluenceZ() const;
    LightCullType GetLightCullType() const;
    void SetNormalsSmoothingThreshold(unsigned int threshhold);
    void Update();

    bool Init(unsigned int vertexCount);
    unsigned int GetVertexCount() const;
    StandardAttributeSet GetAttributeSet() const ;

    Point3Array * GetPostions();
    Vector3Array * GetNormals();
    Color4Array * GetColors();
    UV2Array * GetUVsTexture0();
    UV2Array * GetUVsTexture1();
};

#endif

