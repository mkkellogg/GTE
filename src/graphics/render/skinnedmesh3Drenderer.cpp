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
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
#include "global/global.h"
#include "ui/debug.h"


SkinnedMesh3DRenderer::SkinnedMesh3DRenderer()
{

}

SkinnedMesh3DRenderer::~SkinnedMesh3DRenderer()
{

}

Mesh3DRef SkinnedMesh3DRenderer::GetMesh()
{
	return mesh;
}

SubMesh3DRef SkinnedMesh3DRenderer::GetSubMesh(unsigned int index)
{
	SHARED_REF_CHECK(sceneObject,"SkinnedMesh3DRenderer::GetSubMesh -> sceneObject is NULL.", SubMesh3DRef::Null());

	SHARED_REF_CHECK(mesh,"SkinnedMesh3DRenderer::GetSubMesh -> mesh is NULL.", SubMesh3DRef::Null());

	SubMesh3DRef subMesh = mesh->GetSubMesh(index);
	SHARED_REF_CHECK(subMesh,"SkinnedMesh3DRenderer::GetSubMesh -> subMesh is NULL.", SubMesh3DRef::Null());

	return subMesh;
}

void SkinnedMesh3DRenderer::SetMesh(Mesh3DRef mesh)
{
	this->mesh = mesh;
	UpdateFromMesh();
}

void SkinnedMesh3DRenderer::UpdateFromMesh()
{
	if(mesh.IsValid())
	{
		Mesh3DRenderer::UpdateFromMesh(mesh);
	}
}
