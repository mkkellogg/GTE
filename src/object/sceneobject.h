#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;
class SubMesh3DRenderer;
class SubMesh3D;
class Mesh3DRenderer;
class SkinnedMesh3DRenderer;
class Mesh3D;
class Transform;
class Camera;
class SceneObject;
class Light;

#include "geometry/sceneobjecttransform.h"
#include "engineobject.h"
#include "enginetypes.h"
#include <vector>
#include <memory>

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;
	friend class RenderManager;

	protected:

	std::string name;
	bool isActive;
	Transform transform;
	Transform processingTransform;
	std::vector<SceneObjectRef > children;
	SceneObjectRef parent;
	CameraRef camera;
	LightRef light;
	Mesh3DRendererRef renderer3D;
	SkinnedMesh3DRendererRef skinnedRenderer3D;
	Mesh3DRef mesh3D;

	SceneObject();
    virtual ~SceneObject();

    void SetAggregateTransform(Transform& transform);

	public:

    bool IsActive();
    void SetActive(bool active);
    void SetName(const std::string& name);
    const std::string& GetName();

    Transform& GetLocalTransform() ;
    void InitSceneObjectTransform(SceneObjectTransform * transform);
    const Transform& GetAggregateTransform() const;

    bool SetMesh3DRenderer(Mesh3DRendererRef renderer);
    bool SetSkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer);
    bool SetMesh3D(Mesh3DRef mesh);

    bool SetCamera(CameraRef camera);
    bool SetLight(LightRef light);

    Mesh3DRef GetMesh3D();
    Mesh3DRendererRef GetMesh3DRenderer();
    SkinnedMesh3DRendererRef GetSkinnedMesh3DRenderer();

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
