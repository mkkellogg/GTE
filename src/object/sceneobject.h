#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;
class SubMesh3DRenderer;
class SubMesh3D;
class Mesh3DRenderer;
class Mesh3D;
class Transform;
class Camera;
class SceneObject;
class SceneObjectTransform;
class Light;

#include "engineobject.h"
#include "enginetypes.h"
#include <vector>
#include <memory>

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	bool isActive;
	std::vector<SceneObject * > children;
	SceneObject * parent;
	SceneObjectTransform * transform;
	Camera * camera;
	LightHandle light;
	Mesh3DRenderer * renderer3D;
	Mesh3D * mesh3D;

	SceneObject();
    virtual ~SceneObject();

	public:

    bool IsActive();
    void SetActive(bool active);

    Transform * GetLocalTransform() const ;
    void GetFullTransform(Transform * transform);

    bool SetMeshRenderer3D(Mesh3DRenderer *renderer);
    bool SetMesh3D(Mesh3D *mesh);

    bool SetCamera(Camera * camera);
    bool SetLight(LightHandle light);

    Mesh3D * GetMesh3D();
    Mesh3DRenderer * GetRenderer3D();

    Camera * GetCamera();
    LightHandle GetLight();

    void AddChild(SceneObject * child);
    void RemoveChild(SceneObject * child);
    unsigned int GetChildrenCount() const;
    SceneObject * GetChildAt(unsigned int index) const;
    SceneObject * GetParent();
};

#endif
