#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "skinnedmesh3Drenderer.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/object/mesh3D.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/skinnedmeshattrtransformer.h"
#include "global/global.h"
#include "ui/debug.h"


SkinnedMesh3DRenderer::SkinnedMesh3DRenderer()
{
	meshTransformer = NULL;
}

SkinnedMesh3DRenderer::~SkinnedMesh3DRenderer()
{

}

void SkinnedMesh3DRenderer::SetMesh(Mesh3DRef mesh)
{
	this->mesh = mesh;
}

void SkinnedMesh3DRenderer::UpdateFromMesh()
{
	Mesh3DRenderer::UpdateFromMesh(mesh);
}
