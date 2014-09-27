#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/sceneobjectcomponent.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "material.h"
#include "graphics/graphics.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "ui/debug.h"
#include "global/global.h"


SubMesh3DRenderer::SubMesh3DRenderer(Graphics * graphics) : SceneObjectComponent()
{
	this->material = NULL;
	this->activeMaterial = NULL;
	this->graphics = graphics;
}

SubMesh3DRenderer::~SubMesh3DRenderer()
{

}

bool SubMesh3DRenderer::UpdateMeshData()
{
	return true;
}

bool SubMesh3DRenderer::UseMaterial(Material * material)
{
	NULL_CHECK(material, "Mesh3DRenderer::UseMaterial -> material is NULL", false);
	this->activeMaterial = material;

	return true;
}

Material * SubMesh3DRenderer::GetMaterial()
{
	return material;
}

void SubMesh3DRenderer::SetMaterial(Material * material)
{
	NULL_CHECK_RTRN(material, "Mesh3DRenderer::SetMaterial -> material is NULL");
	this->material = material;
}

