#ifndef _MESH3D_H_
#define _MESH3D_H_

//forward declarations
class EngineObjectManager;
class SubMesh3D;
class SceneObject;
class SceneObjectComponent;

#include "object/engineobject.h"
#include "object/sceneobjectcomponent.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/light/light.h"
#include <vector>

class Mesh3D : public SceneObjectComponent
{
	friend EngineObjectManager;
	friend SubMesh3D;

	protected :

	LightCullType lightCullType;

	Point3 center;
	Vector3 sphereOfInfluenceX;
	Vector3 sphereOfInfluenceY;
	Vector3 sphereOfInfluenceZ;

	unsigned int subMeshCount;
	std::vector<SubMesh3DRef> subMeshes;

    Mesh3D(unsigned int subMeshCount);
    ~Mesh3D();
    void Destroy();

    void SendDataToRenderer(unsigned int subIndex);
    void CalculateSphereOfInfluence();

	public:

    unsigned int GetSubMeshCount();
    void SetSubMesh(SubMesh3DRef mesh, unsigned int index);
    SubMesh3DRef GetSubMesh(unsigned int index);
    bool Init();
    void Update();

    bool IsAttachedToSceneObject();
    bool SceneObjectHasRenderer();

    const Point3 * GetCenter() const;
    const Vector3 * GetSphereOfInfluenceX() const;
    const Vector3 * GetSphereOfInfluenceY() const;
    const Vector3 * GetSphereOfInfluenceZ() const;
    LightCullType GetLightCullType() const;
};

#endif

