#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectskeletonnode.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

SceneObjectSkeletonNode::SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex, const std::string& name) : SkeletonNode(boneIndex, name)
{
	this->Target = target;
}

SceneObjectSkeletonNode::~SceneObjectSkeletonNode()
{

}

const Transform * SceneObjectSkeletonNode::GetFullTransform() const
{
	const Transform& ref = Target->GetProcessingTransform();
	return &ref;
}

Transform * SceneObjectSkeletonNode::GetLocalTransform()
{
	Transform& ref = Target->GetLocalTransform();
	return &ref;
}

bool SceneObjectSkeletonNode::HasTarget() const
{
	return Target.IsValid();
}

SkeletonNode * SceneObjectSkeletonNode::FullClone() const
{
	SkeletonNode * newNode = new SceneObjectSkeletonNode(Target, BoneIndex, Name);
	NULL_CHECK(newNode,"SceneObjectSkeletonNode::FullClone -> Could not allocate new node",NULL);

	return newNode;
}
