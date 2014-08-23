#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

// forward declarations
class Mesh3D;
class Material;

#include "object/sceneobjectcomponent.h"

class Mesh3DRenderer : public SceneObjectComponent
{
	protected:

	Mesh3D * mesh;
	Material * material;

    public:

    Mesh3DRenderer();
    virtual ~Mesh3DRenderer();

    virtual void Render() = 0;
    virtual bool UseMesh(Mesh3D * newMesh);
    Mesh3D * GetMesh();
    virtual bool UseMaterial(Material * material);
    Material * GetMaterial();
};

#endif

