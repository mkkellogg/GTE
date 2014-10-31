#ifndef _SKELETON_NODE_H_
#define _SKELETON_NODE_H_

//forward declarations
class Bone;

#include <vector>
#include <string>

class SkeletonNode
{
	public:

	int BoneIndex;
	std::string Name;

	virtual const Transform * GetFullTransform() const = 0;
	virtual Transform * GetLocalTransform() = 0;
	virtual bool HasTarget() const = 0;

	SkeletonNode(int boneIndex, const std::string& name);
	virtual ~SkeletonNode();

	virtual SkeletonNode * FullClone() const = 0;
};

#endif
