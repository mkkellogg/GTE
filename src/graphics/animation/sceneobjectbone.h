#ifndef _SCENEOBJECT_BONE_H_
#define _SCENEOBJECT_BONE_H_

//forward declarations
class Transform;

#include "bone.h"
#include <vector>
#include <string>
#include "object/enginetypes.h"

class SceneObjectBone : public Bone
{
	SceneObjectRef target;

	public :

	const Transform * GetFullTransform() const;
	Transform * GetLocalTransform();

	SceneObjectBone(SceneObjectRef target);
	SceneObjectBone(SceneObjectRef target, std::string& name);
	SceneObjectBone(SceneObjectRef target, std::string& name, unsigned int id);
     ~SceneObjectBone();
};

#endif
