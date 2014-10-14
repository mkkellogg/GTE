#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectskeletonnode.h"
#include "global/global.h"
#include "ui/debug.h"
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
	const Transform& ref = target->GetProcessingTransform();
	return &ref;
}

Transform * SceneObjectSkeletonNode::GetLocalTransform()
{
	Transform& ref = target->GetLocalTransform();
	return &ref;
}

void SceneObjectSkeletonNode::SetTarget(SceneObjectRef target)
{
	this->target = target;
}

SkeletonNode * SceneObjectSkeletonNode::FullClone() const
{
	SkeletonNode * newNode = new SceneObjectSkeletonNode(target, boneIndex);
	NULL_CHECK(newNode,"SceneObjectSkeletonNode::FullClone -> Could not allocate new node",NULL);

	return newNode;
}
