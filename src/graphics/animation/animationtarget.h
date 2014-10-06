#ifndef _ANIMATION_TARGET_H_
#define _ANIMATION_TARGET_H_

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;
class Bone;

#include <vector>
#include "util/tree.h"

class AnimationTarget
{
	public:

	class AnimationSkeletalNode
	{
		int boneIndex;

		public:

		AnimationSkeletalNode();
		~AnimationSkeletalNode();
	};

	private:

	unsigned int boneCount;
	Bone * bones;
	Tree<AnimationSkeletalNode> skeleton;

	void Destroy();

	public :

	AnimationTarget(unsigned int boneCount);
    ~AnimationTarget();

    bool Init();
    //void SetSkeletonRoot(AnimationSkeletalNode * root);
    Bone * GetBone(unsigned int boneIndex);
};

#endif

