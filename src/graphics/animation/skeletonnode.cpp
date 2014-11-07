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

/*
 * Only constructor.
 */
SkeletonNode::SkeletonNode(int boneIndex, const std::string& name)
{
	this->BoneIndex = boneIndex;
	this->Name = name;
}

/*
 * Destructor.
 */
SkeletonNode::~SkeletonNode()
{

}

