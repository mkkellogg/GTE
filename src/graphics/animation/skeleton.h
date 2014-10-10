#ifndef _SKELETON_H_
#define _SKELETON_H_

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;
class Bone;

#include <vector>
#include <string>
#include <unordered_map>
#include "skeletonnode.h"
#include "util/tree.h"

class Skeleton
{
	private:

	std::unordered_map<std::string, unsigned int> boneNameMap;
	unsigned int boneCount;
	std::vector<Bone *> bones;
	Tree<SkeletonNode> skeleton;

	void Destroy();

	public :

	Skeleton(unsigned int boneCount);
    ~Skeleton();

    bool Init();
    SkeletonNode * CreateSkeletonRoot(unsigned int boneIndex);
    Bone * GetBone(unsigned int boneIndex);
};

#endif

