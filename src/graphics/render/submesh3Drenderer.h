#ifndef _SUBMESH3D_RENDERER_H_
#define _SUBMESH3D_RENDERER_H_

// forward declarations
class SubMesh3D;
class Material;
class Graphics;

#include "object/sceneobjectcomponent.h"

class SubMesh3DRenderer : public EngineObject
{
	friend class SceneObject;
	friend class SubMesh3D;

	protected:

	Material * material;
	Material * activeMaterial;
	Graphics * graphics;

	virtual bool UpdateMeshData();
	virtual bool UseMaterial(Material * material);

    public:

    SubMesh3DRenderer(Graphics * graphics);
    virtual ~SubMesh3DRenderer();

    virtual void Render() = 0;
    virtual void UpdateFromMesh() = 0;
    Material * GetMaterial();
    void SetMaterial(Material * material);
};

#endif

