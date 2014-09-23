#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;
class Mesh3DRenderer;
class Mesh3D;
class Transform;
class Camera;
class SceneObject;
class SceneObjectTransform;
class Light;

#include "engineobject.h"
#include <vector>

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	std::vector<SceneObject * > children;
	SceneObject * parent;

	SceneObjectTransform * transform;

	Mesh3DRenderer * renderer3D;
	Mesh3D * mesh3D;
	Camera * camera;
	Light * light;

	SceneObject();
    virtual ~SceneObject();

	public:

    Transform * GetLocalTransform() const ;
    void GetFullTransform(Transform * transform);

    bool SetMeshRenderer3D(Mesh3DRenderer *renderer);
    bool SetMesh3D(Mesh3D *mesh);
    bool SetCamera(Camera * camera);
    bool SetLight(Light * light);

    Mesh3D * GetMesh3D();
    Mesh3DRenderer * GetRenderer3D();
    Camera * GetCamera();
    Light * GetLight();

    void AddChild(SceneObject * child);
    void RemoveChild(SceneObject * child);
    unsigned int GetChildrenCount() const;
    SceneObject * GetChildAt(unsigned int index) const;
    SceneObject * GetParent();
};

#endif
