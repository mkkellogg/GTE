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

	private:

	std::unordered_map<std::string, unsigned int> boneNameMap;
	unsigned int boneCount;
	Bone * bones;
	std::vector<VertexBoneMap *> vertexBoneMap;
	Tree<SkeletonNode*> skeleton;

	Skeleton(unsigned int boneCount);
	~Skeleton();

	void Destroy();

	public :

    unsigned int GetBoneCount();

    bool Init();
    Tree<SkeletonNode*>::TreeNode * CreateRoot(SkeletonNode* node);
    Tree<SkeletonNode*>::TreeNode * AddChild(Tree<SkeletonNode*>::TreeNode * parent, SkeletonNode* node);

    void MapBone(std::string& name, unsigned int boneIndex);
    int GetBoneMapping(std::string& name);

    Bone* GetBone(unsigned int boneIndex);

    void AddVertexBoneMap(VertexBoneMap * map);
    VertexBoneMap * GetVertexBoneMap(unsigned int index);

    Skeleton * FullClone();
};

#endif

