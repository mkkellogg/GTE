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

		unsigned int BoneCount=0;
		unsigned int BoneIndex[Constants::MaxBonesPerVertex];
		float Weight[Constants::MaxBonesPerVertex];

		void SetTo(VertexMappingDescriptor * desc)
		{
			this->BoneCount = desc->BoneCount;
			memcpy(this->BoneIndex, desc->BoneIndex, sizeof(unsigned int) * Constants::MaxBonesPerVertex);
			memcpy(this->Weight, desc->Weight, sizeof(float) * Constants::MaxBonesPerVertex);
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
	unsigned int GetVertexCount();
	VertexBoneMap * FullClone();
};


#endif
