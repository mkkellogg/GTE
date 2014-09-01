#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "material.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "ui/debug.h"
#include "global/global.h"


Mesh3DRenderer::Mesh3DRenderer(Graphics * graphics) : SceneObjectComponent()
{
	this->mesh = NULL;
	this->material = NULL;
	this->activeMaterial = NULL;
	this->graphics = graphics;
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
	NULL_CHECK(material, "Mesh3DRenderer::UseMaterial -> material is NULL", false);
	this->activeMaterial = material;

	return true;
}

Material * Mesh3DRenderer::GetMaterial()
{
	return material;
}

void Mesh3DRenderer::SetMaterial(Material * material)
{
	NULL_CHECK_RTRN(material, "Mesh3DRenderer::SetMaterial -> material is NULL");
	this->material = material;
}

