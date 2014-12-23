#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectskeletonnode.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

/*
 * Only constructor, which is parameterized.
 */
SceneObjectSkeletonNode::SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex, const std::string& name) : SkeletonNode(boneIndex, name)
{
	this->Target = target;
}

/*
 * Destructor.
 */
SceneObjectSkeletonNode::~SceneObjectSkeletonNode()
{

}

/*
 * This method gets the full transform of the target SceneObject, meaning its local transform
 * concatenated with the transforms of all its ancestors.
 */
const Transform * SceneObjectSkeletonNode::GetFullTransform() const
{
	ASSERT(Target.IsValid(),"SceneObjectSkeletonNode::GetFullTransform -> Node does not have a valid target.", NULL);

	const Transform& ref = Target->GetAggregateTransform();
	return &ref;
}

/*
 * Retrieve only the local transform of the target SceneObject.
 */
Transform * SceneObjectSkeletonNode::GetLocalTransform()
{
	ASSERT(Target.IsValid(),"SceneObjectSkeletonNode::GetLocalTransform -> Node does not have a valid target.", NULL);

	Transform& ref = Target->GetLocalTransform();
	return &ref;
}

/*
 * Is this node pointed at a valid SceneObject target?
 */
bool SceneObjectSkeletonNode::HasTarget() const
{
	return Target.IsValid();
}

/*
 * Create a full (deep) clone of this node.
 */
SkeletonNode * SceneObjectSkeletonNode::FullClone() const
{
	SkeletonNode * newNode = new SceneObjectSkeletonNode(Target, BoneIndex, Name);
	ASSERT(newNode != NULL,"SceneObjectSkeletonNode::FullClone -> Could not allocate new node",NULL);

	return newNode;
}
