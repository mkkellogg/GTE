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
#include <vector>

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	bool isActive;
	std::vector<SceneObject * > children;
	SceneObject * parent;

	SceneObjectTransform * transform;

	SubMesh3DRenderer * subrenderer3D;
	SubMesh3D * submesh3D;
	Camera * camera;
	Light * light;

	Mesh3DRenderer * renderer3D;
	Mesh3D * mesh3D;

	SceneObject();
    virtual ~SceneObject();

	public:

    bool IsActive();
    void SetActive(bool active);

    Transform * GetLocalTransform() const ;
    void GetFullTransform(Transform * transform);

    bool SetSubMeshRenderer3D(SubMesh3DRenderer *renderer);
    bool SetSubMesh3D(SubMesh3D *mesh);
    bool SetMeshRenderer3D(Mesh3DRenderer *renderer);
    bool SetMesh3D(Mesh3D *mesh);

    bool SetCamera(Camera * camera);
    bool SetLight(Light * light);

    SubMesh3D * GetSubMesh3D();
    SubMesh3DRenderer * GetSubRenderer3D();
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
