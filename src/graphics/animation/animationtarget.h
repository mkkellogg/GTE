#ifndef _ANIMATION_TARGET_H_
#define _ANIMATION_TARGET_H_

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;
class Bone;

#include <vector>

class AnimationTarget
{
	class SkeletalNode
	{
		Bone * bone;

		public:

		std::vector <SkeletalNode * > children;

		SkeletalNode();
		~SkeletalNode();
	};

	SkeletalNode * structureRoot;
	unsigned int boneCount;
	Bone ** bones;

	public :

	AnimationTarget(unsigned int boneCount);
    ~AnimationTarget();

    bool Init();
};

#endif

