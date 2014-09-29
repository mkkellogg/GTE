#ifndef _ANIMATION_TARGET_H_
#define _ANIMATION_TARGET_H_

//forward declarations
class Point3Array;
class Vector3Array;
class VertexAttrBuffer;

#include <vector>

class AnimationTarget
{
	class SkeletalNode
	{
		public:

		std::vector <SkeletalNode * > children;

		SkeletalNode()
		{

		}

		~SkeletalNode()
		{

		}
	};

	unsigned int boneCount;
	Bone ** bones;

	public :

	AnimationTarget(unsigned int boneCount);
    ~AnimationTarget();

    bool Init();
};

#endif

