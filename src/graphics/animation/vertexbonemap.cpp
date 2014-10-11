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
	NULL_CHECK_RTRN(mappingDescriptors, "VertexBoneMap::Destroy -> mappingDescriptors is NULL.");

	if(mappingDescriptors != NULL)
	{
		delete[] mappingDescriptors;
		mappingDescriptors = NULL;
	}
}

void VertexBoneMap::Init()
{
	Destroy();

	mappingDescriptors = new VertexMappingDescriptor[vertexCount];
	NULL_CHECK_RTRN(mappingDescriptors, "VertexBoneMap::Init -> unable to allocate vertex mapping descriptors master array.");
}
