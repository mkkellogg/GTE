/*********************************************
*
* class: VertexBoneMap
*
* author: Mark Kellogg
*
* This class stores vertex skinning information. Specifically it maps mesh vertices
* to instances of VertexMappingDescriptor, which contain information about what bone(s)
* the vertices are attached to and the respective weight for each attachment.
*
***********************************************/

#ifndef _VERTEX_BONE_MAP_H_
#define _VERTEX_BONE_MAP_H_

// forward declarations
class Skeleton;

#include "object/enginetypes.h"
#include "global/global.h"
#include "ui/debug.h"
#include "global/constants.h"

class VertexBoneMap
{
	public:

	// this class holds vertex skinning information for a single vertex
	class VertexMappingDescriptor
	{
		public:

		// the unique vertex index, all vertices that have the same x,y,z values will have
		// the same UVertexIndex value
		unsigned int UVertexIndex=0;
		// number of bones to which this vertex is attached
		unsigned int BoneCount=0;
		// bones to which this vertex is attached
		unsigned int BoneIndex[Constants::MaxBonesPerVertex];
		// weight of each bone attachment, should add up to 1
		float Weight[Constants::MaxBonesPerVertex];

		// make this VertexMappingDexcriptor object identical to [desc].
		void SetTo(VertexMappingDescriptor* desc)
		{
			ASSERT_RTRN(desc != NULL,"VertexMappingDescriptor::SetTo -> desc is NULL.");

			this->BoneCount = desc->BoneCount;
			this->UVertexIndex = desc->UVertexIndex;
			memcpy(this->BoneIndex, desc->BoneIndex, sizeof(unsigned int) * Constants::MaxBonesPerVertex);
			memcpy(this->Weight, desc->Weight, sizeof(float) * Constants::MaxBonesPerVertex);
		}
	};

	private:

	// number of unique vertices
	unsigned int uVertexCount;
	// total number of vertices
	unsigned int vertexCount;
	// mapping descriptor for each vertex
	VertexMappingDescriptor * mappingDescriptors;

	void Destroy();

	public:

	VertexBoneMap(unsigned int vertexCount, unsigned int uVertexCount);
	~VertexBoneMap();
	bool Init();
	VertexMappingDescriptor* GetDescriptor(unsigned int index);
	unsigned int GetVertexCount();
	unsigned int GetUVertexCount();
	VertexBoneMap * FullClone();
};


#endif
