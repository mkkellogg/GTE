#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

//forward declarations
class EngineObjectManager;
class SubMesh3DRenderer;
class Material;

#include "object/engineobject.h"
#include <vector>

class Mesh3DRenderer : public SceneObjectComponent
{
	friend EngineObjectManager;

	protected :

	std::vector <Material *> materials;
	std::vector <SubMesh3DRenderer *> renderers;

	Mesh3DRenderer();
    ~Mesh3DRenderer();
    void DestroyRenderers();
    void DestroyRenderer(unsigned int index);

	public:

    unsigned int GetMaterialCount();
    Material * GetMaterial(unsigned int index);
    void SetMaterial(unsigned int index, Material * material);
    void AddMaterial(Material * material);

    void UpdateFromMeshes();
    void UpdateFromMesh(unsigned int index);
};

#endif
