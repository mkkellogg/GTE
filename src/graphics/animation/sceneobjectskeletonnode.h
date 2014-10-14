#ifndef _SCENEOBJECT_BONE_H_
#define _SCENEOBJECT_BONE_H_

//forward declarations
class Transform;

#include "skeletonnode.h"
#include <vector>
#include <string>
#include "object/enginetypes.h"

class SceneObjectSkeletonNode : public SkeletonNode
{
	SceneObjectRef target;

	public :

	const Transform * GetFullTransform() const;
	Transform * GetLocalTransform();

	SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex);
     ~SceneObjectSkeletonNode();

     void SetTarget(SceneObjectRef target);
     SkeletonNode * FullClone() const;
};

#endif
