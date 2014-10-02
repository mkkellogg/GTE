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
	SceneObjectTransform * transform;
	std::vector<SceneObjectRef > children;
	SceneObjectRef parent;
	CameraRef camera;
	LightRef light;
	Mesh3DRenderer * renderer3D;
	Mesh3DRef mesh3D;

	SceneObject();
    virtual ~SceneObject();

	public:

    bool IsActive();
    void SetActive(bool active);

    Transform * GetLocalTransform() const ;
    void GetFullTransform(Transform * transform);

    bool SetMeshRenderer3D(Mesh3DRenderer *renderer);
    bool SetMesh3D(Mesh3DRef mesh);

    bool SetCamera(CameraRef camera);
    bool SetLight(LightRef light);

    Mesh3DRef GetMesh3D();
    Mesh3DRenderer * GetRenderer3D();

    CameraRef GetCamera();
    LightRef GetLight();

    void AddChild(SceneObjectRef child);
    void RemoveChild(SceneObjectRef child);
    unsigned int GetChildrenCount() const;
    SceneObjectRef GetChildAt(unsigned int index) const;
    SceneObjectRef GetParent();

    // TODO: optimize this hashing function (implement correctly)
    typedef struct
	{
		 int operator()(const SceneObject& s) const
		 {
			  return (int)s.GetObjectID() << 1;
		 }
	}SceneObjectHasher;

	typedef struct
	{
	  bool operator() (const SceneObject& a, const SceneObject& b) const { return a==b; }
	} SceneObjectEq;

	bool operator==(const SceneObject& s) const
	{
		return s.GetObjectID() == this->GetObjectID();
	}
};

#endif
