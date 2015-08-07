#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectskeletonnode.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "engine.h"
#include <string>

namespace GTE
{
	/*
	* Only constructor, which is parameterized.
	*/
	SceneObjectSkeletonNode::SceneObjectSkeletonNode(SceneObjectRef target, Int32 boneIndex, const std::string& name) : SkeletonNode(boneIndex, name)
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
		NONFATAL_ASSERT_RTRN(Target.IsValid(), "SceneObjectSkeletonNode::GetFullTransform -> Node does not have a valid target.", nullptr, true);

		const Transform& ref = Target->GetAggregateTransform();
		return &ref;
	}

	/*
	 * Retrieve only the local transform of the target SceneObject.
	 */
	Transform * SceneObjectSkeletonNode::GetLocalTransform()
	{
		NONFATAL_ASSERT_RTRN(Target.IsValid(), "SceneObjectSkeletonNode::GetLocalTransform -> Node does not have a valid target.", nullptr, true);

		Transform& ref = Target->GetTransform();
		return &ref;
	}

	/*
	 * Is this node pointed at a valid SceneObject target?
	 */
	Bool SceneObjectSkeletonNode::HasTarget() const
	{
		return Target.IsValid();
	}

	/*
	 * Create a full (deep) clone of this node.
	 */
	SkeletonNode * SceneObjectSkeletonNode::FullClone() const
	{
		SkeletonNode * newNode = new(std::nothrow) SceneObjectSkeletonNode(Target, BoneIndex, Name);
		ASSERT(newNode != nullptr, "SceneObjectSkeletonNode::FullClone -> Could not allocate new node.");

		newNode->InitialTransform = this->InitialTransform;
		newNode->InitialTranslation = this->InitialTranslation;
		newNode->InitialScale = this->InitialScale;
		newNode->InitialRotation = this->InitialRotation;

		return newNode;
	}
}
