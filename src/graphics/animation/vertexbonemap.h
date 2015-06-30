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

#ifndef _GTE_VERTEX_BONE_MAP_H_
#define _GTE_VERTEX_BONE_MAP_H_

#include "object/enginetypes.h"
#include "geometry/matrix4x4.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include "global/constants.h"

namespace GTE
{
	// forward declarations
	class Skeleton;

	class VertexBoneMap
	{
	public:

		// this class holds vertex skinning information for a single vertex
		class VertexMappingDescriptor
		{
		public:

			// the unique vertex index, all vertices that have the same x,y,z values will have
			// the same UVertexIndex value
			UInt32 UniqueVertexIndex = 0;
			// number of bones to which this vertex is attached
			UInt32 BoneCount = 0;
			// bones to which this vertex is attached
			UInt32 BoneIndex[Constants::MaxBonesPerVertex];
			// weight of each bone attachment, should add up to 1
			Real Weight[Constants::MaxBonesPerVertex];
			// name of the bones, used for rebind to different skeletons
			std::string Name[Constants::MaxBonesPerVertex];

			// make this VertexMappingDexcriptor object identical to [desc].
			void SetTo(VertexMappingDescriptor* desc)
			{
				ASSERT(desc != NULL, "VertexMappingDescriptor::SetTo -> 'desc' is null.");

				this->BoneCount = desc->BoneCount;
				this->UniqueVertexIndex = desc->UniqueVertexIndex;
				memcpy(this->BoneIndex, desc->BoneIndex, sizeof(UInt32) * Constants::MaxBonesPerVertex);
				memcpy(this->Weight, desc->Weight, sizeof(Real) * Constants::MaxBonesPerVertex);
				for (UInt32 b = 0; b < Constants::MaxBonesPerVertex; b++)
				{
					Name[b] = desc->Name[b];
				}
			}
		};

	private:

		// number of unique vertices
		UInt32 uniqueVertexCount;
		// total number of vertices
		UInt32 vertexCount;
		// mapping descriptor for each vertex
		VertexMappingDescriptor * mappingDescriptors;

		void Destroy();
		VertexBoneMap * FullClone();

	public:

		VertexBoneMap(UInt32 vertexCount, UInt32 uVertexCount);
		~VertexBoneMap();
		Bool Init();
		VertexMappingDescriptor* GetDescriptor(UInt32 index);
		UInt32 GetVertexCount() const;
		UInt32 GetUniqueVertexCount() const;
		void BindTo(SkeletonRefConst skeleton);
	};
}

#endif
