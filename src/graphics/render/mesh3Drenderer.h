#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

// forward declarations
class Mesh3D;
class Material;
class Graphics;

#include "object/sceneobjectcomponent.h"

class Mesh3DRenderer : public SceneObjectComponent
{
	friend class SceneObject;
	friend class Mesh3D;

	protected:

	Mesh3D * mesh;
	Material * material;
	Material * activeMaterial;
	Graphics * graphics;

	virtual bool UseMesh(Mesh3D * newMesh);
	virtual bool UseMaterial(Material * material);

    public:

    Mesh3DRenderer(Graphics * graphics);
    virtual ~Mesh3DRenderer();

    virtual void Render() = 0;
    Mesh3D * GetMesh();
    virtual void UpdateFromMesh() = 0;
    Material * GetMaterial();
    void SetMaterial(Material * material);
};

#endif

