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

	protected:

	Mesh3D * mesh;
	Material * material;
	Graphics * graphics;

	virtual bool UseMesh(Mesh3D * newMesh);

    public:

    Mesh3DRenderer(Graphics * graphics);
    virtual ~Mesh3DRenderer();

    virtual void Render() = 0;
    Mesh3D * GetMesh();
    virtual bool UseMaterial(Material * material);
    Material * GetMaterial();
};

#endif

