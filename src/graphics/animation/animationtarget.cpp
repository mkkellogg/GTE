#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationtarget.h"

AnimationTarget::AnimationTarget(unsigned int boneCount)
{
	this->boneCount = boneCount;
	bones = NULL;
}

AnimationTarget::~AnimationTarget()
{

}

bool AnimationTarget::Init()
{
	return true;
}

