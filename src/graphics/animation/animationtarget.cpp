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

AnimationTarget::AnimationSkeletalNode::AnimationSkeletalNode(int boneIndex)
{
	this->boneIndex = boneIndex;
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
		delete node;
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

AnimationTarget::AnimationSkeletalNode * AnimationTarget::CreateSkeletonRoot(unsigned int boneIndex)
{
	if(!skeleton.GetRoot())
	{
		AnimationTarget::AnimationSkeletalNode * tempRoot = new AnimationTarget::AnimationSkeletalNode(boneIndex);
		NULL_CHECK(tempRoot,"AnimationTarget::SetSkeletonRoot -> Unable to allocate new AnimationSkeletalNode.", NULL);

		skeleton.AddRoot(tempRoot);
	}
	return skeleton.GetRoot();
}

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
