#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexbonemap.h"
#include "skeleton.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "global/global.h"
#include "debug/gtedebug.h"

/*
 * Only constructor, parameterized.
 */
VertexBoneMap::VertexBoneMap(unsigned int vertexCount, unsigned int uVertexCount)
{
	this->vertexCount = vertexCount;
	this->uniqueVertexCount = uVertexCount;
	mappingDescriptors = NULL;
}

/*
 * Destructor.
 */
VertexBoneMap::~VertexBoneMap()
{
	Destroy();
}

/*
 * Destroy this VertexBoneMap object.
 */
void VertexBoneMap::Destroy()
{
	// delete [mappingDescriptors] and set to NULL
	if(mappingDescriptors != NULL)
	{
		delete[] mappingDescriptors;
		mappingDescriptors = NULL;
	}
}

/*
 * Initialize this VertexBoneMap object. This method MUST be called before this object can be used.
 */
bool VertexBoneMap::Init()
{
	// destroy existing data, if it exists
	Destroy();

	mappingDescriptors = new VertexMappingDescriptor[vertexCount];
	ASSERT(mappingDescriptors != NULL, "VertexBoneMap::Init -> unable to allocate vertex mapping descriptors master array.", false);

	return true;
}

/*
 * Get VertexMappingDescriptor for vertex (non-unique) at [index]
 */
VertexBoneMap::VertexMappingDescriptor* VertexBoneMap::GetDescriptor(unsigned int index)
{
	if(index >= vertexCount)
	{
		Debug::PrintError("VertexBoneMap::GetDescriptor -> Index out of range.");
		return NULL;
	}

	return mappingDescriptors + index;
}

/*
 * Get total number of vertices that are mapped.
 */
unsigned int VertexBoneMap::GetVertexCount()
{
	return vertexCount;
}

/*
 * Get number of unique vertices that are mapped.
 */
unsigned int VertexBoneMap::GetUniqueVertexCount()
{
	return uniqueVertexCount;
}

/*
 * Update the bone indices in this map to match that of [skeleton]
 */
void VertexBoneMap::BindTo(SkeletonRef skeleton)
{
	ASSERT_RTRN(skeleton.IsValid(), "VertexBoneMap::BindTo -> skeleton is not valid.");

	for(unsigned int v = 0; v < vertexCount; v++)
	{
		VertexBoneMap::VertexMappingDescriptor * desc = GetDescriptor(v);
		for(unsigned int b = 0; b < desc->BoneCount; b++)
		{
			int boneIndex = skeleton->GetBoneMapping(desc->Name[b]);
			desc->BoneIndex[b] = boneIndex;
		}
	}
}

/*
 * Create a full (deep) clone of this VertexBoneMap object.
 */
VertexBoneMap * VertexBoneMap::FullClone()
{
	// allocate new VertexBoneMap objects
	VertexBoneMap * clone = new VertexBoneMap(vertexCount, uniqueVertexCount);
	ASSERT(clone != NULL, "VertexBoneMap::FullClone -> Could not allocate vertex bone map.", NULL);

	// initialize the new map
	bool initSuccess = clone->Init();
	if(!initSuccess)
	{
		Debug::PrintError("VertexBoneMap::FullClone -> Could not initialize vertex bone map.");
		delete clone;
		return NULL;
	}

	// copy over VertexMappingDescriptors one-by-one
	for(unsigned int v = 0; v < vertexCount; v++)
	{
		clone->GetDescriptor(v)->SetTo(GetDescriptor(v));
	}

	return clone;
}
