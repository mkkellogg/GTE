#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "skinnedmesh3Drenderer.h"
#include "submesh3Drenderer.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "graphics/animation/skeleton.h"
#include "graphics/object/mesh3D.h"
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

void SkinnedMesh3DRenderer::SetSkeleton(SkeletonRef skeleton)
{
	this->skeleton = skeleton;

	for(unsigned int i = 0; i< mesh->GetSubMeshCount(); i++)
	{
		SubMesh3DRendererRef subRenderer =  this->GetSubRenderer(i);

		if(subRenderer->GetAttributeTransformer() != NULL)
		{
			SkinnedMesh3DAttributeTransformer * attrTransformer = dynamic_cast<SkinnedMesh3DAttributeTransformer*>(subRenderer->GetAttributeTransformer());
			if(attrTransformer != NULL)
			{
				attrTransformer->SetSkeleton(skeleton);
			}
		}
	}
}

SkeletonRef SkinnedMesh3DRenderer::GetSkeleton()
{
	return skeleton;
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

		for(unsigned int i = 0; i< mesh->GetSubMeshCount(); i++)
		{
			int vertexBoneMapIndex = -1;
			SubMesh3DRendererRef subRenderer =  this->GetSubRenderer(i);

			if(subMeshIndexMap.find(i) != subMeshIndexMap.end())
			{
				vertexBoneMapIndex = subMeshIndexMap[i];

				if(subRenderer->GetAttributeTransformer() == NULL)
				{
					StandardAttributeSet attributes = StandardAttributes::CreateAttributeSet();
					StandardAttributes::AddAttribute(&attributes, StandardAttribute::Position);
					StandardAttributes::AddAttribute(&attributes, StandardAttribute::Normal);

					SkinnedMesh3DAttributeTransformer *attributeTransformer = new SkinnedMesh3DAttributeTransformer(attributes);
					NULL_CHECK_RTRN(attributeTransformer,"SkinnedMesh3DRenderer::UpdateFromMesh -> Could not allocate attribute transformer.");

					attributeTransformer->SetSkeleton(skeleton);
					attributeTransformer->SetVertexBoneMapIndex(vertexBoneMapIndex);

					subRenderer->SetAttributeTransformer(attributeTransformer);
				}
			}
			else subRenderer->SetAttributeTransformer(NULL);
		}
	}
}

void SkinnedMesh3DRenderer::MapSubMeshToVertexBoneMap(unsigned int subMeshIndex, unsigned int vertexBoneMapIndex)
{
	subMeshIndexMap[subMeshIndex] = vertexBoneMapIndex;
}
