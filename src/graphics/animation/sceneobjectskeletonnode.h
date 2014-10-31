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
	public :

	SceneObjectRef Target;

	const Transform * GetFullTransform() const;
	Transform * GetLocalTransform();
	bool HasTarget() const;

	SceneObjectSkeletonNode(SceneObjectRef target, int boneIndex, const std::string& name);
     ~SceneObjectSkeletonNode();

     SkeletonNode * FullClone() const;
};

#endif
