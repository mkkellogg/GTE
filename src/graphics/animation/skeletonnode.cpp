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

SkeletonNode::SkeletonNode(int boneIndex)
{
	this->boneIndex = boneIndex;
}

SkeletonNode::~SkeletonNode()
{

}
