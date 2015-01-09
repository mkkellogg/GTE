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

//forward declarations
class Transform;

#include "skeletonnode.h"
#include <vector>
#include <string>
#include "object/enginetypes.h"

class SceneObjectSkeletonNode : public SkeletonNode
{
	public :

	// target SceneObject
	SceneObjectRef Target;

	SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex, const std::string& name);
     ~SceneObjectSkeletonNode();

 	const Transform * GetFullTransform() const;
 	Transform * GetLocalTransform();
     bool HasTarget() const;
     SkeletonNode * FullClone() const;
};

#endif
