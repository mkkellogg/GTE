#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationtarget.h"

AnimationTarget::SkeletalNode::SkeletalNode()
{
	bone = NULL;
}

AnimationTarget::SkeletalNode::~SkeletalNode()
{

}

AnimationTarget::AnimationTarget(unsigned int boneCount)
{
	this->boneCount = boneCount;
	bones = NULL;
	structureRoot = NULL;
}

AnimationTarget::~AnimationTarget()
{

}

bool AnimationTarget::Init()
{
	return true;
}

