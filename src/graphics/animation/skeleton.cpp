#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <memory>
#include "skeleton.h"
#include "skeletonnode.h"
#include "bone.h"
#include "util/tree.h"
#include "global/global.h"
#include "ui/debug.h"

Skeleton::Skeleton(unsigned int boneCount)
{
	this->boneCount = boneCount;
}

Skeleton::~Skeleton()
{
	Destroy();
}

void Skeleton::Destroy()
{
	for(unsigned int i =0; i < bones.size(); i++)
	{
		if(bones[i] != NULL)
		{
			delete bones[i];
		}
	}
	bones.clear();

	skeleton.SetTraversalCallback([](SkeletonNode * node) -> bool
	{
		delete node;
		return false;
	});

	skeleton.Traverse();
}

bool Skeleton::Init()
{
	//bones = new Bone[boneCount];
	//NULL_CHECK(bones,"AnimationTarget::Init -> unable to allocate bones array.", false);

	return true;
}

SkeletonNode *  Skeleton::CreateSkeletonRoot(unsigned int boneIndex)
{
	if(skeleton.GetRoot() == NULL)
	{
		SkeletonNode * tempRoot = new SkeletonNode(boneIndex);
		NULL_CHECK(tempRoot,"AnimationTarget::SetSkeletonRoot -> Unable to allocate new AnimationSkeletalNode.", NULL);

		skeleton.AddRoot(tempRoot);
	}
	return skeleton.GetRoot()->Data;
}

Bone * Skeleton::GetBone(unsigned int boneIndex)
{
	if(boneIndex >= boneCount)
	{
		Debug::PrintError("AnimationTarget::GetBone -> Index is out of range.");
		return NULL;
	}

	return bones[boneIndex];
}
