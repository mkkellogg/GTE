#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <memory>
#include "animationtarget.h"
#include "bone.h"
#include "util/tree.h"
#include "global/global.h"
#include "ui/debug.h"

AnimationTarget::AnimationSkeletalNode::AnimationSkeletalNode()
{
	boneIndex = -1;
}

AnimationTarget::AnimationSkeletalNode::~AnimationSkeletalNode()
{

}

AnimationTarget::AnimationTarget(unsigned int boneCount)
{
	this->boneCount = boneCount;
	bones = NULL;
}

AnimationTarget::~AnimationTarget()
{
	Destroy();
}

void AnimationTarget::Destroy()
{
	if(bones != NULL)
	{
		delete[] bones;
	}
	bones = NULL;

	skeleton.SetTraversalCallback([](AnimationSkeletalNode * node) -> bool
	{
		AnimationTarget::AnimationSkeletalNode * lastNode = node;

		delete lastNode;

		return false;
	});

	skeleton.Traverse();
}

bool AnimationTarget::Init()
{
	bones = new Bone[boneCount];
	NULL_CHECK(bones,"AnimationTarget::Init -> unable to allocate bones array.", false);

	return true;
}

/*void AnimationTarget::SetSkeletonRoot(AnimationSkeletalNode * root)
{
	NULL_CHECK_RTRN(root,"AnimationTarget::SetSkeletonRoot -> root is NULL.");
	this->skeletonRoot = root;
}*/

Bone * AnimationTarget::GetBone(unsigned int boneIndex)
{
	NULL_CHECK(bones,"AnimationTarget::GetBone -> root is NULL.", NULL);

	if(boneIndex >= boneCount)
	{
		Debug::PrintError("AnimationTarget::GetBone -> Index is out of range.");
		return NULL;
	}

	return &(bones[boneIndex]);
}
