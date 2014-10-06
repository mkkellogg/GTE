#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "skinnedmesh3Dvertexmap.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "global/global.h"
#include "ui/debug.h"

SkinnedMesh3DVertexMap::SkinnedMesh3DVertexMap()
{
	mappingDescriptors = NULL;
	animationTarget = NULL;
}

SkinnedMesh3DVertexMap::~SkinnedMesh3DVertexMap()
{
	Destroy();
}

void SkinnedMesh3DVertexMap::Destroy()
{
	SHARED_REF_CHECK_RTRN(targetMesh,"SkinnedMesh3DVertexMap::Destroy -> targetMesh is NULL.");
	NULL_CHECK_RTRN(mappingDescriptors, "SkinnedMesh3DVertexMap::Destroy -> mappingDescriptors is NULL.");

	unsigned int subMeshCount = targetMesh->GetSubMeshCount();
	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		if(mappingDescriptors[i] != NULL)
		{
			delete[] mappingDescriptors[i];
			mappingDescriptors[i] = NULL;
		}
	}
	SAFE_DELETE(mappingDescriptors);
}

void SkinnedMesh3DVertexMap::Init(Mesh3DRef targetMesh)
{
	Destroy();

	this->targetMesh = targetMesh;
	SHARED_REF_CHECK_RTRN(targetMesh,"SkinnedMesh3DVertexMap::Init -> targetMesh is NULL.");

	unsigned int subMeshCount = targetMesh->GetSubMeshCount();

	mappingDescriptors = new VertexMappingDescriptor*[subMeshCount];
	NULL_CHECK_RTRN(mappingDescriptors, "SkinnedMesh3DVertexMap::Init -> unable to allocate vertex mapping descriptors master array.");

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		mappingDescriptors[i] = NULL;
	}

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		SubMesh3DRef subMesh = targetMesh->GetSubMesh(i);
		if(!subMesh.IsValid())
		{
			Debug::PrintWarning("SkinnedMesh3DVertexMap::Init -> subMesh is NULL.");
		}
		else
		{
			unsigned int vertexCount = subMesh->GetVertexCount();
			mappingDescriptors[i] = new VertexMappingDescriptor[vertexCount];

			if(mappingDescriptors[i] == NULL)
			{
				Debug::PrintWarning("SkinnedMesh3DVertexMap::Init ->  unable to allocate vertex mapping descriptors sub array.");
				Destroy();
				return;
			}
		}
	}
}
