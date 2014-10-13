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

	virtual const Transform * GetFullTransform() const = 0;
	virtual Transform * GetLocalTransform() = 0;

	SkeletonNode(int boneIndex);
	virtual ~SkeletonNode();
};

#endif
