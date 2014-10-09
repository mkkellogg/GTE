#ifndef _SKINNED_MESH3D_VERTEX_MAP_H_
#define _SKINNED_MESH3D_VERTEX_MAP_H_

// forward declarations
class Skeleton;

#include "object/enginetypes.h"
#include "global/constants.h"

class SkinnedMesh3DVertexMap
{
	class VertexMappingDescriptor
	{
		public:

		unsigned int boneCount=0;
		unsigned int boneIndex[Constants::MaxBonesPerVertex];
		float weight[Constants::MaxBonesPerVertex];
	};

	Skeleton * animationTarget;
	Mesh3DRef targetMesh;
	VertexMappingDescriptor ** mappingDescriptors;

	void Destroy();

	public:

	SkinnedMesh3DVertexMap();
	~SkinnedMesh3DVertexMap();
	void Init(Mesh3DRef targetMesh);
};


#endif
