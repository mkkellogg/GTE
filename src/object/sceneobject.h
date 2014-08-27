#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;
class Mesh3DRenderer;
class Mesh3D;
class Transform;
class Camera;
class SceneObject;

#include "engineobject.h"
#include <vector>

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	std::vector<SceneObject * > children;

	Transform * transform;

	Mesh3DRenderer * renderer3D;
	Mesh3D * mesh3D;
	Camera * camera;

	SceneObject();
    virtual ~SceneObject();

	public:

    Transform * GetTransform() const ;

    bool SetMeshRenderer(Mesh3DRenderer *renderer);
    bool SetMesh(Mesh3D *mesh);
    bool SetCamera(Camera * camera);

    Mesh3DRenderer * GetRenderer3D();
    Camera * GetCamera();

    void AddChild(SceneObject * child);
    unsigned int GetChildrenCount() const;
    SceneObject * GetChildAt(int index) const;
};

#endif
