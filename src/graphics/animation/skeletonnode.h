#ifndef _SKELETON_NODE_H_
#define _SKELETON_NODE_H_

//forward declarations
class Bone;

#include <vector>
#include <string>

class SkeletonNode
{
	int boneIndex;

	public:

	SkeletonNode(int boneIndex);
	~SkeletonNode();
};

#endif
