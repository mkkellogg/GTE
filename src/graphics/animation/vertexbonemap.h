#ifndef _VERTEX_BONE_MAP_H_
#define _VERTEX_BONE_MAP_H_

// forward declarations
class Skeleton;

#include "object/enginetypes.h"
#include "global/constants.h"

class VertexBoneMap
{
	class VertexMappingDescriptor
	{
		public:

		unsigned int boneCount=0;
		unsigned int boneIndex[Constants::MaxBonesPerVertex];
		float weight[Constants::MaxBonesPerVertex];
	};

	unsigned int vertexCount;
	VertexMappingDescriptor * mappingDescriptors;

	void Destroy();

	public:

	VertexBoneMap(unsigned int vertexCount);
	~VertexBoneMap();
	void Init();
};


#endif
