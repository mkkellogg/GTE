/*********************************************
*
* class: SkeletonNode
*
* author: Mark Kellogg
*
* The SkeletonNode class is the base class for nodes in a Skeleton object.
* Skeleton objects describe a transformation hierarchy, but are agnostic to
* the kind of objects that hold the transformation information. Classes
* that inherit from SkeletonNode must implement two methods that make
* available the Transform objects of their target: GetFullTransform() and
* GetLocalTransform().
*
***********************************************/

#ifndef _SKELETON_NODE_H_
#define _SKELETON_NODE_H_

//forward declarations
class Bone;

#include <vector>
#include <string>

class SkeletonNode
{
	public:

	// the index of the corresponding bone (if there is one) in the container Skeleton object's
	// bone array.
	int BoneIndex;
	// the name of this node
	std::string Name;

	SkeletonNode(int boneIndex, const std::string& name);
	virtual ~SkeletonNode();

	virtual const Transform * GetFullTransform() const = 0;
	virtual Transform * GetLocalTransform() = 0;
	virtual bool HasTarget() const = 0;

	virtual SkeletonNode * FullClone() const = 0;
};

#endif
