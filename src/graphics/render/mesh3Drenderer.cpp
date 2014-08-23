#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "material.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "ui/debug.h"


Mesh3DRenderer::Mesh3DRenderer()
{
	this->mesh = NULL;
	this->material = NULL;
}

Mesh3DRenderer::~Mesh3DRenderer()
{

}

bool Mesh3DRenderer::UseMesh(Mesh3D * newMesh)
{
	this->mesh = newMesh;

	return true;
}

Mesh3D * Mesh3DRenderer::GetMesh()
{
	return mesh;
}

bool Mesh3DRenderer::UseMaterial(Material * material)
{
	this->material = material;

	return true;
}

Material * Mesh3DRenderer::GetMaterial()
{
	return material;
}

