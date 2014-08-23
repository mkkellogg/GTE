#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

class Mesh3DRenderer;

#include "graphics/object/mesh3D.h"
#include "vertexattrbuffer.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "material.h"

class Mesh3DRenderer
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

