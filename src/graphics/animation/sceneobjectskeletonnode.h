/*********************************************
*
* class: SceneObjectNode
*
* author: Mark Kellogg
*
* This class inherits from SkeletonNode and provides functionality
* for a node that specifically targets SceneObjects.
*
***********************************************/

#ifndef _GTE_SCENEOBJECT_BONE_H_
#define _GTE_SCENEOBJECT_BONE_H_

#include <string>

#include "engine.h"
#include "skeletonnode.h"

namespace GTE
{
	//forward declarations
	class Transform;

	class SceneObjectSkeletonNode : public SkeletonNode
	{
	public:

		// target SceneObject
		SceneObjectSharedPtr Target;

		SceneObjectSkeletonNode(SceneObjectRef target, Int32 boneIndex, const std::string& name);
		~SceneObjectSkeletonNode();

		const Transform * GetFullTransform() const;
		Transform * GetLocalTransform();
		Bool HasTarget() const;
		SkeletonNode * FullClone() const;
	};
}

#endif
