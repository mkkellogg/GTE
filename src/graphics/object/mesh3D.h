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

class Mesh3D : public SceneObjectComponent
{
	friend EngineObjectManager;
	friend SubMesh3D;

	protected :

	unsigned int subMeshCount;
	SubMesh3DRef * subMeshes;

    Mesh3D(unsigned int subMeshCount);
    ~Mesh3D();
    void SendDataToRenderer(unsigned int subIndex);

	public:

    unsigned int GetSubMeshCount();
    void SetSubMesh(SubMesh3DRef mesh, unsigned int index);
    SubMesh3DRef GetSubMesh(unsigned int index);
    bool Init();
    void Update();

    bool IsAttachedToSceneObject();
    bool SceneObjectHasRenderer();
};

#endif

