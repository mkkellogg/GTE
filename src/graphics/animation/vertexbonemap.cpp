#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "vertexbonemap.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "global/global.h"
#include "ui/debug.h"

VertexBoneMap::VertexBoneMap(unsigned int vertexCount)
{
	this->vertexCount = vertexCount;
	mappingDescriptors = NULL;
}

VertexBoneMap::~VertexBoneMap()
{
	Destroy();
}

void VertexBoneMap::Destroy()
{
	if(mappingDescriptors != NULL)
	{
		delete[] mappingDescriptors;
		mappingDescriptors = NULL;
	}
}

bool VertexBoneMap::Init()
{
	Destroy();

	mappingDescriptors = new VertexMappingDescriptor[vertexCount];
	NULL_CHECK(mappingDescriptors, "VertexBoneMap::Init -> unable to allocate vertex mapping descriptors master array.", false);

	return true;
}

VertexBoneMap::VertexMappingDescriptor* VertexBoneMap::GetDescriptor(unsigned int index)
{
	if(index >= vertexCount)
	{
		Debug::PrintError("VertexBoneMap::GetDescriptor -> Index out of range.");
		return NULL;
	}

	return mappingDescriptors + index;
}
