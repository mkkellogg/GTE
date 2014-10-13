#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectskeletonnode.h"
#include <string>

SceneObjectSkeletonNode::SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex) : SkeletonNode(boneIndex)
{
	this->target = target;
}

SceneObjectSkeletonNode::~SceneObjectSkeletonNode()
{

}

const Transform * SceneObjectSkeletonNode::GetFullTransform() const
{
	return &(target->GetProcessingTransform());
}

Transform * SceneObjectSkeletonNode::GetLocalTransform()
{
	return &(target->GetLocalTransform());
}
