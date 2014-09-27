#ifndef _MESH3D_H_
#define _MESH3D_H_

//forward declarations
class EngineObjectManager;
class SubMesh3D;
class SceneObject;
class SceneObjectComponent;

#include "object/engineobject.h"
#include "object/sceneobjectcomponent.h"

class Mesh3D : public SceneObjectComponent
{
	friend EngineObjectManager;

	protected :

	unsigned int subMeshCount;
	SubMesh3D ** subMeshes;

    Mesh3D(unsigned int subMeshCount);
    ~Mesh3D();

	public:

    unsigned int GetSubMeshCount();
    void SetSubMesh(SubMesh3D * mesh, unsigned int index);
    SubMesh3D * GetSubMesh(unsigned int index);
    bool Init();

    void Update(SubMesh3D * subMesh);
    void Update();
};

#endif

