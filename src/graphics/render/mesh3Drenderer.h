#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

//forward declarations
class EngineObjectManager;
class SubMesh3DRenderer;
class SubMesh3D;
class Material;
class Mesh3D;

#include "object/engineobject.h"
#include <vector>

class Mesh3DRenderer : public SceneObjectComponent
{
	friend EngineObjectManager;
	friend SubMesh3DRenderer;

	protected :

	std::vector <MaterialRef> materials;
	std::vector <SubMesh3DRendererRef> subRenderers;

	Mesh3DRenderer();
    ~Mesh3DRenderer();
    void DestroyRenderers();
    void DestroyRenderer(unsigned int index);

	public:

    unsigned int GetMaterialCount();
    MaterialRef GetMaterial(unsigned int index);
    void SetMaterial(unsigned int index, MaterialRef material);
    void AddMaterial(MaterialRef material);

    virtual void UpdateFromMeshes();
    void UpdateFromMeshes(Mesh3DRef mesh);
    void UpdateFromMesh(unsigned int index);

    Mesh3DRef GetMesh();
    SubMesh3DRef GetSubMeshForSubRenderer(SubMesh3DRendererRef subRenderer);
    SubMesh3DRef GetSubMesh(unsigned int index);
    SubMesh3DRendererRef GetSubRenderer(unsigned int index);
    unsigned int GetSubRendererCount();
};

#endif
