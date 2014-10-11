#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/sceneobject.h"
#include "sceneobjectbone.h"
#include <string>

SceneObjectBone::SceneObjectBone(SceneObjectRef target)
{
	this->target = target;
}

SceneObjectBone::SceneObjectBone(SceneObjectRef target, std::string& name) : Bone(name)
{
	this->target = target;
}

SceneObjectBone::SceneObjectBone(SceneObjectRef target, std::string& name, unsigned int id) : Bone(name,id)
{
	this->target = target;
}

SceneObjectBone::~SceneObjectBone()
{

}

const Transform * SceneObjectBone::GetFullTransform() const
{
	return &(target->GetProcessingTransform());
}

Transform * SceneObjectBone::GetLocalTransform()
{
	return &(target->GetLocalTransform());
}
