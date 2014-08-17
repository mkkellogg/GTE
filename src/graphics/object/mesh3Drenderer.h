#ifndef _MESH3D_RENDERER_H_
#define _MESH3D_RENDERER_H_

#include "mesh3D.h"

#include "graphics/vertexattrbuffer.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"

#include "geometry/point3array.h"
#include "geometry/vector3array.h"
#include "graphics/color4array.h"
#include "graphics/uv2array.h"

class Mesh3DRenderer
{
	Mesh3D * mesh;

    bool buffersOnGPU;

    public:

    Mesh3DRenderer();
    Mesh3DRenderer(bool buffersOnGPU);
    virtual ~Mesh3DRenderer();

    virtual void Render() = 0;
    virtual bool UseMesh(Mesh3D * newMesh);
    Mesh3D * GetMesh();
};

#endif

