#ifndef _VERTEX_BONE_MAP_H_
#define _VERTEX_BONE_MAP_H_

// forward declarations
class Skeleton;

#include "object/enginetypes.h"
#include "global/constants.h"

class VertexBoneMap
{
	public:

	class VertexMappingDescriptor
	{
		public:

		unsigned int boneCount=0;
		unsigned int boneIndex[Constants::MaxBonesPerVertex];
		float weight[Constants::MaxBonesPerVertex];

		void SetTo(VertexMappingDescriptor * desc)
		{
			this->boneCount = desc->boneCount;
			memcpy(this->boneIndex, desc->boneIndex, sizeof(unsigned int) * Constants::MaxBonesPerVertex);
			memcpy(this->weight, desc->weight, sizeof(float) * Constants::MaxBonesPerVertex);
		}
	};

	private:

	unsigned int vertexCount;
	VertexMappingDescriptor * mappingDescriptors;

	void Destroy();

	public:

	VertexBoneMap(unsigned int vertexCount);
	~VertexBoneMap();
	bool Init();
	VertexMappingDescriptor* GetDescriptor(unsigned int index);
};


#endif
