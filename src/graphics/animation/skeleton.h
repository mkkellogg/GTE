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

#ifndef _SKELETON_H_
#define _SKELETON_H_

//forward declarations
class Bone;
class VertexBoneMap;
class Transform;

#include <vector>
#include <string>
#include <unordered_map>
#include "object/engineobject.h"
#include "skeletonnode.h"
#include "util/tree.h"

class Skeleton : public EngineObject
{
	friend class EngineObjectManager;

	// number of bones in this skeleton
	unsigned int boneCount;
	// indexed list of all the bones in this skeleton
	Bone * bones;
	// map from bone name to index in [bones] for the matching Bone object
	std::unordered_map<std::string, unsigned int> boneNameMap;
	// all the VertexBoneMap objects for this skeleton
	std::vector<VertexBoneMap *> vertexBoneMaps;

	// map from node name to index in [nodeList] for the mtching SkeletonNode object
	std::unordered_map<std::string, unsigned int> nodeNameMap;
	// indexed list of all the nodes in this skeleton
	std::vector<SkeletonNode *> nodeList;

	// contains transformation hierarchy structure
	Tree<SkeletonNode*> skeleton;

	Skeleton(unsigned int boneCount);
	~Skeleton();

	void Destroy();

	public :

    unsigned int GetBoneCount();
    unsigned int GetNodeCount();

    bool Init();
    Tree<SkeletonNode*>::TreeNode * CreateRoot(SkeletonNode* node);
    Tree<SkeletonNode*>::TreeNode * AddChild(Tree<SkeletonNode*>::TreeNode * parent, SkeletonNode* node);

    void MapBone(std::string& name, unsigned int boneIndex);
    int GetBoneMapping(std::string& name);
    Bone* GetBone(unsigned int boneIndex);

    void MapNode(std::string& name, unsigned int nodeIndex);
    int GetNodeMapping(std::string& name);
    SkeletonNode * GetNodeFromList(unsigned int nodeIndex);
    void AddNodeToList(SkeletonNode * node);


    void AddVertexBoneMap(VertexBoneMap * map);
    VertexBoneMap * GetVertexBoneMap(unsigned int index);

    Skeleton * FullClone();
};

#endif

