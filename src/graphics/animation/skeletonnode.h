/*********************************************
*
* class: SkeletonNode
*
* author: Mark Kellogg
*
* The SkeletonNode class is the base class for nodes in a Skeleton object.
* Skeleton objects describe a transformation hierarchy, but are agnostic to
* the kind of objects that hold the transformation information. Classes
* that inherit from SkeletonNode must implement two methods that make
* available the Transform objects of their target: GetFullTransform() and
* GetLocalTransform().
*
***********************************************/

#ifndef _GTE_SKELETON_NODE_H_
#define _GTE_SKELETON_NODE_H_

#include <vector>
#include <string>

#include "engine.h"
#include "geometry/matrix4x4.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"

namespace GTE
{
	//forward declarations
	class Bone;

	class SkeletonNode
	{
	public:

		// the index of the corresponding bone (if there is one) in the container Skeleton object's
		// bone array.
		Int32 BoneIndex;
		// the name of this node
		std::string Name;
		// save the original transformations
		Matrix4x4 InitialTransform;
		Vector3 InitialTranslation;
		Vector3 InitialScale;
		Quaternion InitialRotation;

		SkeletonNode(Int32 boneIndex, const std::string& name);
		virtual ~SkeletonNode();

		virtual const Transform * GetFullTransform() const = 0;
		virtual Transform * GetLocalTransform() = 0;
		virtual Bool HasTarget() const = 0;

		virtual SkeletonNode * FullClone() const = 0;
	};
}

#endif
