#ifndef _SUBMESH3D_RENDERER_H_
#define _SUBMESH3D_RENDERER_H_

// forward declarations
class SubMesh3D;
class Material;
class Graphics;
class Mesh3DRenderer;

#include "object/sceneobjectcomponent.h"

class SubMesh3DRenderer : public EngineObject
{
	friend class SceneObject;
	friend class SubMesh3D;
	friend class Mesh3DRenderer;

	protected:

	Material * material;
	Material * activeMaterial;
	Graphics * graphics;
	Mesh3DRenderer * containerRenderer;

	virtual bool UpdateMeshData();
	virtual bool UseMaterial(Material * material);
	void SetContainerRenderer(Mesh3DRenderer * renderer);

    public:

    SubMesh3DRenderer(Graphics * graphics);
    virtual ~SubMesh3DRenderer();

    virtual void Render() = 0;
    virtual void UpdateFromMesh() = 0;
    Material * GetMaterial();
    void SetMaterial(Material * material);
};

#endif

