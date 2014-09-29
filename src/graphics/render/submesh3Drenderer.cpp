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


SubMesh3DRenderer::SubMesh3DRenderer(Graphics * graphics) : EngineObject()
{
	this->activeMaterial = NULL;
	this->graphics = graphics;
	this->containerRenderer = NULL;
}

SubMesh3DRenderer::~SubMesh3DRenderer()
{

}

void SubMesh3DRenderer::SetContainerRenderer(Mesh3DRenderer * renderer)
{
	NULL_CHECK_RTRN(renderer, "SubMesh3DRenderer::SetContainerRenderer -> renderer is NULL");
	this->containerRenderer = renderer;
}

bool SubMesh3DRenderer::UpdateMeshData()
{
	return true;
}

bool SubMesh3DRenderer::UseMaterial(Material * material)
{
	NULL_CHECK(material, "SubMesh3DRenderer::UseMaterial -> material is NULL", false);
	this->activeMaterial = material;

	return true;
}

