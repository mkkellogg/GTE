/*********************************************
*
* class: Skeleton
*
* author: Mark Kellogg
*
* A Skeleton encapsulates a hierarchy/tree of transformations, each of which is encapsulated
* by a SkeletonNode object. These nodes don't necessarily contain the transformation information
* themselves (although they might via polymorphism), and may instead point to other objects
* that contain actual transformation information.
*
* A Skeleton object also contains bones (of course). Bones contain additional information beyond
* hierarchy transformation information, such as an offset transformation which moves
* from model space into bone space, and is used when performing vertex skinning. All bones have
* a single corresponding node in the skeleton, but a node does not have to have a bone.
*
***********************************************/

#ifndef _GTE_SKELETON_H_
#define _GTE_SKELETON_H_

#include "engine.h"
#include "object/engineobject.h"
#include "skeletonnode.h"
#include "util/tree.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace GTE
{
	//forward declarations
	class Bone;
	class VertexBoneMap;
	class Transform;

	class Skeleton : public EngineObject
	{
		friend class EngineObjectManager;

		// number of bones in this skeleton
		UInt32 boneCount;
		// indexed list of all the bones in this skeleton
		Bone * bones;
		// map from bone name to index in [bones] for the matching Bone object
		std::unordered_map<std::string, UInt32> boneNameMap;

		// map from node name to index in [nodeList] for the mtching SkeletonNode object
		std::unordered_map<std::string, UInt32> nodeNameMap;

		// indexed list of all the nodes in this skeleton
		std::vector<SkeletonNode *> nodeList;

		// contains transformation hierarchy structure
		Tree<SkeletonNode*> skeleton;

		Skeleton(UInt32 boneCount);
		~Skeleton();

		void Destroy();
		Skeleton * FullClone();

	public:

		UInt32 GetBoneCount() const;
		UInt32 GetNodeCount() const;

		Bool Init();
		Tree<SkeletonNode*>::TreeNode * CreateRoot(SkeletonNode* node);
		Tree<SkeletonNode*>::TreeNode * AddChild(Tree<SkeletonNode*>::TreeNode * parent, SkeletonNode* node);

		void MapBone(const std::string& name, UInt32 boneIndex);
		Int32 GetBoneMapping(const std::string& name) const;
		Bone* GetBone(UInt32 boneIndex);

		void MapNode(std::string& name, UInt32 nodeIndex);
		Int32 GetNodeMapping(const std::string& name) const;
		SkeletonNode * GetNodeFromList(UInt32 nodeIndex);
		void AddNodeToList(SkeletonNode * node);

		void OverrideBonesFrom(SkeletonConstRef skeleton, Bool takeOffset, Bool takeNode);
		void OverrideBonesFrom(const Skeleton * skeleton, Bool takeOffset, Bool takeNode);
	};
}

#endif

