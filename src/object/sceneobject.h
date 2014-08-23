#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

//forward declarations
class EngineObjectManager;
class Mesh3DRenderer;
class Mesh3D;

#include "engineobject.h"

class SceneObject : public EngineObject
{
	friend class EngineObjectManager;

	protected:

	Mesh3DRenderer * renderer3D;
	Mesh3D * mesh3D;

	SceneObject();
    virtual ~SceneObject();
    void UpdateRenderManager();

	public:

    bool AddMeshRenderer(Mesh3DRenderer *renderer);
    bool AddMesh(Mesh3D *mesh);
    Mesh3DRenderer * GetRenderer3D();
};

#endif
